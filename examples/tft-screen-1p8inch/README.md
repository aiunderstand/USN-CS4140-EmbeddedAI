The 1.8" 128x160 TFT uses an ST7735R controller which requires the MIPI-DBI bus interface. Zephyr has a built-in driver for it. The TFT display shows an image in the RGB565 format which is identical what can be expected from the OV7670 camera sensor. In addition, a typical drawing function for computer vision applications is added: drawing a bounding box around an object interest (as an overlay) with a label and confidence value. 

Design choice: `uart30` is disabled and `spi30` is used instead. On the nRF54L15, `uart30` and `spi30` share the same peripheral address block (`0x104000`) and the same domain (domain-30 / gpiote30), which can drive Port 0 (P0.xx) pins. `spi22` (domain-20 / gpiote20) cannot access Port 0, so it is also disabled. RESET is tied high in hardware via 10 kΩ — no MCU pin required.

| Display Pin | nRF54L15 DK Pin | Function     | Notes                              |
|-------------|-----------------|--------------|-------------------------------------|
| SCK         | P0.00           | SPI Clock    | spi30 (domain-30) — uart30 disabled |
| SDA/MOSI    | P0.01           | SPI Data     | spi30 (domain-30) — uart30 disabled |
| CS          | P0.02           | Chip Select  | spi30 (domain-30) — uart30 disabled |
| DC/A0       | P0.03           | Data/Command | spi30 (domain-30) — uart30 disabled |
| RESET       | 3.3V via 10 kΩ  | Hardware Reset | No MCU pin needed — tie high      |
| VCC         | 3.3V            | Power        |                                     |
| GND         | GND             | Ground       |                                     |
| LED/BL      | 3.3V            | Backlight    |                                     |
