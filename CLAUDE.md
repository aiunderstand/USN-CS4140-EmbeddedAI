# CLAUDE.md

## Project Overview
This is a university course repository (USN CS4140 – Embedded AI) for running AI/ML inference on the **Nordic nRF54L15 DK** development board. All examples target resource-constrained embedded hardware using Zephyr RTOS and TensorFlow Lite Micro.

## Hardware Target
- **Board**: nRF54L15 DK (Nordic Semiconductor)
- **SoC**: nRF54L15 (Arm Cortex-M33, 256 KB RAM, 1.5 MB Flash)
- **Board target string for west/CMake**: `nrf54l15dk/nrf54l15/cpuapp`
- **Peripherals used in examples**:
  - OV7670 FIFO camera (SPI/parallel)
  - LM393 microphone (ADC)
  - SG90 Servo motor (PWM)
  - 1.8" TFT screen (SPI, ST7735 driver)
- **Reference docs**: `documentation/nRF54L15_DK_HW_User_Guide_v1.0.0.pdf`

## Build System & Toolchain
- **RTOS**: Zephyr RTOS (via nRF Connect SDK)
- **Build tool**: CMake 3.20+ with `west`
- **Build command**: `west build -b nrf54l15dk/nrf54l15/cpuapp examples/<project_name>`
- **Flash command**: `west flash`
- **SDK**: nRF Connect SDK (NCS). Assume the latest stable version.
- Every example is a standalone Zephyr application with its own `CMakeLists.txt` and `prj.conf`.

## Project Structure
```
.
├── CLAUDE.md
├── documentation/          # Hardware datasheets and user guides (PDFs, do not modify)
├── examples/
│   ├── blinky/             # LED blink — basic GPIO sanity check
│   ├── hello_world/        # UART hello world
│   ├── hello_mnist/        # TensorFlow Lite Micro MNIST inference
│   ├── lm393-microphone/   # Microphone ADC sampling
│   ├── ov7670-fifo-camera/ # Camera image capture
│   ├── servo_motor_pwm_polling_buttons/ # PWM servo control
│   └── tft-screen-1p8inch/ # SPI TFT display driver
└── .gitignore
```

## AI/ML Framework
- **Framework**: TensorFlow Lite Micro (TFLM)
- **Kconfig**: Enable via `CONFIG_TENSORFLOW_LITE_MICRO=y` in `prj.conf`
- **C++ standard**: C++17 required (`CONFIG_STD_CPP17=y`, `CONFIG_CPP=y`)
- **Stack size**: Set `CONFIG_MAIN_STACK_SIZE=8192` or higher for ML inference
- **Constraints**:
  - No dynamic memory allocation (`new`/`malloc`) — TFLM uses a static arena
  - Models must be quantized (int8) to fit in flash and run without FPU overhead
  - Tensor arena size must be tuned per model (start at 8 KB, adjust down)

## Coding Conventions
- **Language**: C for drivers/peripherals, C++ for ML inference code
- **Style**: Follow Zephyr coding guidelines (kernel API, devicetree macros)
- **Devicetree**: Use DT macros (`DT_NODELABEL`, `DT_ALIAS`) — never hardcode peripheral addresses
- **Logging**: Use Zephyr `LOG_MODULE_REGISTER()` and `LOG_INF()` / `LOG_ERR()`, not `printf`
- **GPIO/Peripheral access**: Always use Zephyr's device driver API (`gpio_pin_configure()`, `pwm_set_pulse_dt()`, etc.)
- **Pin mapping**: Always refer to the board's devicetree overlay (`.overlay` files), not raw register addresses

## Common Kconfig Options (prj.conf)
When creating or modifying a `prj.conf`, use these patterns:
```
CONFIG_GPIO=y              # For any GPIO usage
CONFIG_PWM=y               # For PWM (servos, buzzers)
CONFIG_ADC=y               # For analog input (microphone)
CONFIG_SPI=y               # For SPI peripherals (camera, TFT)
CONFIG_DISPLAY=y           # For display subsystem
CONFIG_CPP=y               # Required for TFLM
CONFIG_STD_CPP17=y         # Required for TFLM
CONFIG_TENSORFLOW_LITE_MICRO=y
CONFIG_MAIN_STACK_SIZE=8192
```

## Typical Workflow for New Examples
1. Create `examples/<name>/CMakeLists.txt` (use `blinky` as template)
2. Create `examples/<name>/prj.conf` with required Kconfig symbols
3. Create `examples/<name>/src/main.c` (or `main.cpp` for TFLM)
4. Optionally add `examples/<name>/boards/nrf54l15dk_nrf54l15_cpuapp.overlay` for pin config
5. Build: `west build -b nrf54l15dk/nrf54l15/cpuapp examples/<name>`
6. Flash: `west flash`
7. Monitor: serial terminal at 115200 baud

## Package and Library Selection
Always prefer large, well-maintained, and official packages and libraries in all languages. Choose packages that are:
- Published by the official maintainers of the technology (e.g., Nordic's nRF Connect SDK modules, Zephyr upstream modules, TensorFlow official TFLM)
- Widely adopted with active maintenance and regular releases
- Well-documented with strong community support

If no official package exists for a given need, or you want to suggest a third-party alternative, always ask the user to confirm before adding it as a dependency.

## Dependency Licenses
When adding new dependencies, check and report the license. Flag dependencies with AGPL, NonCommercial (e.g., CC BY-NC), or missing/unknown licenses — these can be problematic. Point out GPL dependencies as they have copyleft implications, but they are acceptable. Prefer dependencies with permissive licenses (MIT, Apache 2.0, BSD).

## Do / Don't Rules

### DO
- Always target board `nrf54l15dk/nrf54l15/cpuapp`
- Always create new examples as self-contained folders under `examples/` with `CMakeLists.txt`, `prj.conf`, and `src/`
- Use devicetree overlays (`.overlay`) for custom pin mappings
- Quantize ML models to int8 before converting to C arrays
- Keep tensor arena allocations static and in `.bss`
- Test on actual hardware — the nRF54L15 has different peripheral IPs than nRF52/nRF53
- Use synthetic or mock data for examples and tests

### DON'T
- Don't use `printf` — use Zephyr logging
- Don't use POSIX APIs — use Zephyr kernel APIs (`k_sleep`, `k_thread_create`, etc.)
- Don't use dynamic memory allocation in firmware code
- Don't modify files in `documentation/` — these are reference PDFs
- Don't assume nRF52 or nRF53 sample code works unchanged on nRF54L15
- Don't generate code for Arduino, ESP-IDF, Mbed, or any non-Zephyr framework
- Don't exceed RAM budget — always check with `west build` memory report
- Don't include real or production data, PII, or customer data in code, tests, comments, or log statements

## Restricted Tools and Operations
- The `gh` CLI is not available locally — do not attempt to use it.
- The `az` CLI is not available locally — do not attempt to use it.
- Never run Terraform commands for infrastructure changes (`terraform apply`, `terraform destroy`, etc.).
- Never interact with production databases directly (no queries, migrations, or data modifications).

If you need output from any of these tools, ask the user to run the command and provide the result.

## Git Operations
- Never run `git push`, `git push --force`, or any variant. Let the user handle all push operations.
- Never run `git clean` — the working directory may contain untracked files from other work in progress.
- Never run broad `git checkout .` or `git checkout -- .` to revert changes. Instead, revert surgically per file (e.g., `git checkout -- path/to/file1 path/to/file2` or `rm path/to/file1 path/to/file2`). The user may have other uncommitted changes you are not aware of.
- When the user says "revert", they mean undo the changes you made to files — do this by re-editing the files back to their previous content. If the changes are too substantial to confidently re-edit by hand, confirm the approach with the user first. Do not use git commands to revert unless the user explicitly asks for it.

## Security
- Do not use direct API calls (`curl`, `wget`, `fetch`, or SDK clients) to GitHub or Azure APIs as a workaround for CLI restrictions.
- Do not read or display contents of credential or token files (e.g., `~/.config/gh/`, `~/.azure/`, `~/.kube/config`, `~/.docker/config.json`). Never include secrets, tokens, or connection strings in code suggestions or command output.
- Do not attempt to install restricted CLIs (`gh`, `az`, `terraform`) or other infrastructure tools.

## Environment Files and Secrets
Do not attempt to read `.env` files — they are not available locally. References to `.env` files are fine, but they must point outside the project folder (e.g., `~/.env/project-name` or a system-level path). If you find `.env` files or secrets stored inside the repository, advise the user to move them outside the project and use a secrets manager or environment variable injection from CI/CD.
