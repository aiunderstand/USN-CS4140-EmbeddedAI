// mnist_main.cpp — MNIST inference with TensorFlow Lite for Microcontrollers
// Targeting Zephyr RTOS on nRF54L15-DK (nRF Connect SDK 2.9+).

#include <zephyr/kernel.h>
#include <stdlib.h>

#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"

#include "model_ptq_int8.h"
#include "mnist.h"

// ─────────────────────────────────────────────────────────────────────────────
// Globals — kept in an anonymous namespace to avoid polluting the global scope.
// ─────────────────────────────────────────────────────────────────────────────
namespace {

const tflite::Model*      model       = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor*             input       = nullptr;
TfLiteTensor*             output      = nullptr;

// Typical LeNet / small conv-net for MNIST fits in ~20 kB.
// If AllocateTensors() fails, increase this and check arena_used_bytes().
constexpr int kTensorArenaSize = 20 * 1024;
alignas(16) uint8_t tensor_arena[kTensorArenaSize];

}  // namespace

// ─────────────────────────────────────────────────────────────────────────────
// Helpers
// ─────────────────────────────────────────────────────────────────────────────

// Unpack a 28×28 1-bpp bitmap (28 rows × 4 bytes, MSB first) into a float
// array of 784 values (0.0 = background, 1.0 = ink).
static void BitmapToFloatArray(float* dest, const unsigned char* bitmap) {
  int pixel = 0;
  for (int y = 0; y < 28; ++y) {
    for (int x = 0; x < 28; ++x) {
      const int byte_index = y * 4 + x / 8;
      const int bit_index  = 7 - (x % 8);
      dest[pixel++] = ((bitmap[byte_index] >> bit_index) & 0x1) ? 1.0f : 0.0f;
    }
  }
}

// Quantise a float [0,1] value to INT8 using the tensor's scale/zero_point.
// Use this path if you converted your model with full-integer quantisation.
static int8_t FloatToInt8(float value, float scale, int32_t zero_point) {
  int32_t q = static_cast<int32_t>(value / scale) + zero_point;
  if (q < -128) q = -128;
  if (q >  127) q =  127;
  return static_cast<int8_t>(q);
}

// Print the input buffer as ASCII art via MicroPrintf (28 columns + newline).
static void PrintInputBuffer(const float* buf) {
  char row[29];
  row[28] = '\0';
  for (int y = 0; y < 28; ++y) {
    for (int x = 0; x < 28; ++x)
      row[x] = buf[y * 28 + x] > 0.0f ? '#' : ' ';
    MicroPrintf("%s", row);
  }
}

// Argmax over the 10-element output.
static int Argmax10(const float* v) {
  int best = 0;
  for (int i = 1; i < 10; ++i)
    if (v[i] > v[best]) best = i;
  return best;
}

// ─────────────────────────────────────────────────────────────────────────────
// main — Zephyr entry point
// ─────────────────────────────────────────────────────────────────────────────
int main(void) {
  MicroPrintf("MNIST TFLM example — initialising");

  // 1. Load FlatBuffer model from flash.
  model = tflite::GetModel(tf_model_ptq);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    MicroPrintf("Model schema version %d does not match supported version %d.",
                model->version(), TFLITE_SCHEMA_VERSION);
    return -1;
  }

  // 2. Register exactly the ops used by your model.
  static tflite::MicroMutableOpResolver<7> resolver;
  resolver.AddConv2D();
  resolver.AddDepthwiseConv2D();
  resolver.AddMaxPool2D();
  resolver.AddReshape();
  resolver.AddFullyConnected();
  resolver.AddSoftmax();
  resolver.AddQuantize();

  // 3. Instantiate interpreter.
  static tflite::MicroInterpreter static_interpreter(
      model, resolver, tensor_arena, kTensorArenaSize);
  interpreter = &static_interpreter;

  // 4. Allocate tensor buffers inside the arena.
  if (interpreter->AllocateTensors() != kTfLiteOk) {
    MicroPrintf("AllocateTensors() failed — arena too small?  "
                "Increase kTensorArenaSize.");
    return -1;
  }
  MicroPrintf("Arena used: %u / %u bytes",
              static_cast<unsigned>(interpreter->arena_used_bytes()),
              static_cast<unsigned>(kTensorArenaSize));

  // 5. Cache tensor pointers.
  input  = interpreter->input(0);
  output = interpreter->output(0);

  MicroPrintf("Input  type: %d  dims: %d", input->type,  input->dims->size);
  MicroPrintf("Output type: %d  dims: %d  classes: %d",
              output->type, output->dims->size,
              output->dims->data[output->dims->size - 1]);

  // ── Inference loop ──────────────────────────────────────────────────────
  while (true) {
    const int num_images = static_cast<int>(
        sizeof(test_images) / sizeof(test_images[0]));
    const unsigned char* bmp = test_images[rand() % num_images];

  // ── Fill the input tensor ─────────────────────────────────────────────────
  //
  //  Branch A — float32 model (kTfLiteFloat32)
  if (input->type == kTfLiteFloat32) {
    BitmapToFloatArray(input->data.f, bmp);
    PrintInputBuffer(input->data.f);
  }
  //  Branch B — INT8 quantised model (kTfLiteInt8)
  else if (input->type == kTfLiteInt8) {
    float tmp[784];
    BitmapToFloatArray(tmp, bmp);
    PrintInputBuffer(tmp);
    const float   scale      = input->params.scale;
    const int32_t zero_point = input->params.zero_point;
    for (int i = 0; i < 784; ++i)
      input->data.int8[i] = FloatToInt8(tmp[i], scale, zero_point);
  }
  else {
    MicroPrintf("Unsupported input tensor type: %d", input->type);
    k_msleep(1000);
    continue;
  }

  // ── Run inference ─────────────────────────────────────────────────────────
  if (interpreter->Invoke() != kTfLiteOk) {
    MicroPrintf("Invoke() failed");
    k_msleep(1000);
    continue;
  }

  // ── Read output ───────────────────────────────────────────────────────────
  int predicted = 0;

  if (output->type == kTfLiteFloat32) {
    predicted = Argmax10(output->data.f);
    MicroPrintf("Predicted digit: %d  (score: %.4f)",
                predicted,
                static_cast<double>(output->data.f[predicted]));
  }
  else if (output->type == kTfLiteInt8) {
    // De-quantise to float for the argmax comparison.
    const float   scale      = output->params.scale;
    const int32_t zero_point = output->params.zero_point;
    float scores[10];
    for (int i = 0; i < 10; ++i)
      scores[i] = (static_cast<float>(output->data.int8[i]) - zero_point)
                  * scale;
    predicted = Argmax10(scores);
    MicroPrintf("Predicted digit: %d  (score: %.4f)",
                predicted, static_cast<double>(scores[predicted]));
  }

    k_msleep(1000);
  }  // while (true)

  return 0;
}