The 1.8" 128x160 TFT uses an ST7735R controller which requires the MIPI-DBI bus interface. Zephyr has a built-in driver for it.

Design choice: choose pins from P1 and avoid P1.11 since used for microphone. Note this example requires that GPIO run off 3.3V rails which needs to be set in the nRFconnect Board Configuator app.

| Display Pin | nRF54L15 DK Pin | Function     |
|-------------|-----------------|--------------|
| SCK         | P1.13           | SPI Clock    |
| SDA/MOSI    | P1.14           | SPI Data     |
| CS          | P1.10           | Chip Select  |
| DC/A0       | P1.12           | Data/Command |
| RESET       | P1.08           | Reset        |
| VCC         | 3.3V            | Power        |
| GND         | GND             | Ground       |
| LED/BL      | 3.3V            | Backlight    |
