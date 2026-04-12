Wiring schematic for the LM393 Microphone to the nRF54L15 DK
OUT to P1.13
VCC to VDD of P1 (on the far left of headers)
GND to GND of P1 (pin above VDD of P1)

You might need to twist the potentiometer such that the LED on the microphone board is ON when sound is detected (usually a middle value is a good value)

How the code works
------------------
- The devicetree overlay (boards/*.overlay) maps P1.13 to an ADC channel and
  sets gain, reference voltage, and resolution.
- At compile time, ADC_DT_SPEC_GET_BY_IDX reads that config into a const struct.
- At runtime, adc_channel_setup_dt() configures the hardware to match.
- An adc_sequence struct holds a pointer to the sample buffer and
  resolution/channel mask (filled in by adc_sequence_init_dt() from the overlay).
- Each loop iteration calls adc_read_dt() to trigger a conversion, then
  adc_raw_to_millivolts_dt() converts the raw value using the overlay's
  gain and reference settings. The range depends on the voltage rail which is set to 3.3V using the nRF Connect for Desktop Board Configurator