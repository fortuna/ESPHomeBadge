# ESPHomeBadge

<img src="https://github.com/user-attachments/assets/eb86677c-2066-4c75-a571-058af3f2be2c" width="300px" align="right">

[![Status: Work in Progress](https://img.shields.io/badge/Status-Work%20in%20Progress-yellow)](https://img.shields.io/badge/Status-Work%20in%20Progress-yellow)
[![Build Status](https://github.com/fortuna/ESPHomeBadge/actions/workflows/test.yaml/badge.svg)](https://github.com/fortuna/ESPHomeBadge/actions/workflows/test.yaml?query=branch%3Amain)

> **Note:** This project is currently under active development. Expect changes and potential issues.

This repository provides a way to program the [HOPE XV Electronic Badge] ([archive][Badge Archive]) using [ESPHome]. ESPHome is a powerful yet simple framework for controlling your ESP32-based devices.

By [Vinicius Fortuna](https://www.viniciusfortuna.com).

[HOPE XV Electronic Badge]: https://wiki.hope.net/index.php?title=HOPE_XV_Electronic_Badge
[Badge Archive]: https://web.archive.org/web/20241214102029/https://wiki.hope.net/index.php?title=HOPE_XV_Electronic_Badge
[ESPHome]: https://esphome.io/

<br clear="right">

## Why ESPHome for the HOPE Badge?

By enabling users to program the badge with ESPHome, this project aims to:

*   **Simplify Development:** ESPHome's YAML-based configuration and built-in components make it easier to get started with custom functionality.
*   **Increase Accessibility:** Lower the barrier to entry for hacking and customizing the HOPE Badge.
*   **Foster Community:** Encourage collaboration and sharing of custom badge configurations.
* **Leverage Home Assistant:** Integrate the badge into your Home Assistant setup.

## Features (Planned/In Development)

### Hardware Definition

GPIO Components
* [x] Tactile Buttons (SW3/SW4/SW1/SW2, GPIO19/GPIO18/GPIO10/GPIO09, [ESPHome](https://esphome.io/components/binary_sensor.html))
* [x] Green LED (D28, GPIO13, [ESPHome](https://esphome.io/components/output/ledc.html))
* [x] [WS2812B] RGB LED strip (D1-D16, GPIO08, [ESPHome](https://esphome.io/components/light/esp32_rmt_led_strip))
* [x] [IRM-H638] IR receiver (IC1, GPIO03, [ESPhome](https://esphome.io/components/remote_receiver.html))
* [x] [VSMY1850] IR transmitter (D23, GPIO02, [ESPHome](https://esphome.io/components/remote_transmitter.html))
* [x] Vibration motor (GPIO13/SPIWP, [ESPHome](https://esphome.io/components/output/ledc.html))

[WS2812B]: https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf
[IRM-H638]: https://www.everlight.com/wp-content/plugins/ItemRelationship/product_files/pdf/DMO-0000004-IRM-H6XX-TR2_Series_Datasheet_V4.pdf
[VSMY1850]: https://www.vishay.com/docs/83397/vsmy1850.pdf

I2C Components
* [x] [MAX17048] Battery Sensor (U4, address 0x36, [ESPHome](https://esphome.io/components/sensor/max17043.html))
    * Note: We can use the MAX17043 ESPHome component because MAX17048 is a drop-in replacement for MAX17043.
* [ ] [ST25DV04K] Dynamic NFC/RFID tag IC (U6)
    * Access to user memory, Dynamic registers or Mailbox: 0x53
    * System configuration area: 0x57
    * ???: 0x2D
* [x] [SGP30] Air Quality Sensor (U7, address 0x58, [ESPHome](https://esphome.io/components/sensor/sgp30.html))
* [ ] [PCF8574] GPIO Expander (U9, [ESPHome](https://esphome.io/components/pcf8574.html))
* [ ] [ATECC608A] Cryptographic Co-Processor with Secure Hardware-Based Key Storage (U11)

[MAX17048]: https://www.analog.com/media/en/technical-documentation/data-sheets/max17048-max17049.pdf
[ST25DV04K]: https://www.st.com/resource/en/datasheet/st25dv04k.pdf
[SGP30]: https://sensirion.com/media/documents/984E0DD5/61644B8B/Sensirion_Gas_Sensors_Datasheet_SGP30.pdf
[PCF8574]: https://www.ti.com/lit/ds/symlink/pcf8574.pdf
[ATECC608A]: https://ww1.microchip.com/downloads/aemDocuments/documents/SCBU/ProductDocuments/DataSheets/ATECC608A-CryptoAuthentication-Device-Summary-Data-Sheet-DS40001977B.pdf

SPI Components
* [ ] [CAT25640] 64 Kb EEPROM (U8)
* [x] [ST7789V] TFT LCD display driver (J7, [ESPHome](https://esphome.io/components/display/ili9xxx#ili9xxx))

<!-- See https://www.jx-wisevision.com/2-40-small-size-240-rgbx320-dots-tft-lcd-display-module-screen-product/ -->

[CAT25640]: https://www.onsemi.com/pdf/datasheet/cat25640-d.pdf
[ST7789V]: https://newhavendisplay.com/content/datasheets/ST7789V.pdf

### Application

* Remote Access
  * [x] [Wifi provisioning](https://esphome.io/components/wifi.html)
  * [x] [Web Server](https://esphome.io/components/web_server.html) controller
  * [x] [OTA update](https://esphome.io/components/ota/) support
* Add application pages
  * [x] Badge screen
  * [x] Basic air sensor screen
  * [x] QR Code screen. Use the [MeCard format](https://en.wikipedia.org/wiki/MeCard_(QR_code)) for contact info.
  * [x] Basic IR Blaster
  * [ ] LED control and effects
  * [ ] IR Transmitter/Receiver
* Make it easier to add application pages
  * [x] Basic encapsulation of application pages
  * [ ] Remove need for a focus group per page
  * [ ] Create selector entry template
  * [ ] Create page template
  * [ ] Write documentation on how to flash
  * [ ] Write documentation on how to add a new page
  
## Getting Started

1.  **Prerequisites:**
    *   A HOPE XV Electronic Badge.
    *   A computer with ESPHome installed. (See the [ESPHome Getting Started Guide](https://esphome.io/guides/getting_started_command_line.html))
    *   USB cable to connect the badge to your computer.
2.  **Customization (Optional):**
    * Customize the badge to your liking by editing the `firmware_no_screen/main.yaml` or `firmware_screen/main.yaml` configuration file.
3.  **Flashing:**
    *   Connect the HOPE Badge to your computer.
    *   Make sure to hold button 4 while turning on the badge, to enter Firmware Download mode and to prevent
        conflicts with the USB (see below).
    *   Use the ESPHome command-line tool to compile and upload your configuration to the badge:
        ```bash
        esphome run firmware_no_screen/main.yaml
        ```
        or if you have the badge with a screen:
        ```bash
        esphome run firmware_screen/main.yaml
        ```
    *   Turn off the badge, disconnect from USB, and turn it back on.

### Known Flashing Issue

The badge has these conflicts between the tactile buttons and the USB data pins:
- Button 1 (SW3/TACT_C) uses GPIO19, which is shared with USB_D+
- Button 2 (SW4/TACT_D) uses GPIO18, which is shared with USB_D-

The buttons require setting the pin for input, with a pull-down resistor. That breaks the USB connection. As a result, the computer won't even detect the device, preventing flashing and log monitoring.

To successfully flash, you must put it in flash mode:
1. Turn off the device
1. Hold Button 4 (SW2/TACT_B). This uses GPIO09, the BOOT pin
1. Turn on the device
1. Release the Button

This will enter firmware download mode and prevent the firmware from running and breaking the USB connection.

Once flashing is complete, you won't see any logs, because as soon as the firmware starts, it breaks USB.

Also, the firmware won't run correctly somehow. You need to reset it (turn it off and back on).

If you don't disconnect the USB cable, you will see that buttons 1 and 2 will not work. You must disconnect the cable.

## Contributing

Contributions are welcome! If you have ideas, bug fixes, or new features, please open an issue or submit a pull request.

For development, you can set a fixed build cache to speed up builds with:
```bash
export PLATFORMIO_BUILD_CACHE_DIR=/tmp/esphome_cache
```

<!-- It's also helpful to use the `_dev` firmware versions. They enable OTA and disable the conflicting buttons, so you
can see the serial logs. -->

For development of the screen firmware, use `firmware_screen/main_host.yaml` to run LVGL on your host machine.
That will significantly accelerate UI development.

<details>
<summary>
ESP32-C3 Pin Usage on Hope Badge
</summary>

Here is a table summarizing the ESP32-C3 pins based on the ESP32-C3 datasheet and the badge schematic. The "Usage in Badge" column details how the pin is connected in the schematic, and "Availability" indicates if the pin is easily accessible for general use on this specific badge design.

<br>

| Pin No. | Pin Name (Datasheet)        | Usage in Badge (Schematic)               |
| :------ | :-------------------------- | :----------------------------------------|
| 1       | LNA_IN                      | RF Antenna Input                         |
| 2       | VDD3P3                      | +3.3V Power Supply                       |
| 3       | VDD3P3                      | +3.3V Power Supply                       |
| 4       | XTAL_32K_P / GPIO0          | TFT DC (`TFT_DC`)                        |
| 5       | XTAL_32K_N / GPIO1          | TFT Reset (`SPI_CS2`)                    |
| 6       | GPIO2                       | IR Transmitter (`IR_TX`)                 |
| 7       | CHIP_EN                     | Chip Enable Control                      |
| 8       | GPIO3                       | IR Receiver Input (`IR_RX`)              |
| 9       | MTMS / GPIO4                | TFT Chip Select (`SPI_CS1`)              |
| 10      | MTDI / GPIO5                | SPI Data In (`SPI_SDI`)                  |
| 11      | VDD3P3_RTC                  | +3.3V RTC Power Supply                   |
| 12      | MTCK / GPIO6                | SPI Clock (`SPI_SCK`)                    |
| 13      | MTDO / GPIO7                | SPI Data Out (`SPI_SDO`)                 |
| 14      | GPIO8                       | RGB LED Data (`RGB_DAT`)                 |
| 15      | GPIO9 / BOOT                | Tactile Button 4 (SW2, `TACT_B`)         |
| 16      | GPIO10                      | Tactile Button 3 (SW1, `TACT_A`)         |
| 17      | VDD3P3_CPU                  | +3.3V CPU Power Supply                   |
| 18      | VDD_SPI / GPIO11            | SPI Flash Power Supply                   |
| 19      | SPIHD / GPIO12              | Vibration Motor (`VIBRAMOTOR`)           |
| 20      | SPIWP / GPIO13              | LED (D28)                                |
| 21      | SPICS0 / GPIO14             | Internal Flash Chip Select               |
| 22      | SPICLK / GPIO15             | Internal Flash Clock                     |
| 23      | SPID / GPIO16               | Internal Flash Data Out (MOSI)           |
| 24      | SPIQ / GPIO17               | Internal Flash Data In (MISO)            |
| 25      | GPIO18 / USB_D-             | Tactile Button 2 (SW4, `TACT_D`)         |
| 26      | GPIO19 / USB_D+             | Tactile Button 1 (SW3, `TACT_C`)         |
| 27      | U0RXD / GPIO20              | I2C SDA (`I2C_SDA`)                      |
| 28      | U0TXD / GPIO21              | I2C SCL (`I2C_SCL`)                      |
| 29      | XTAL_N                      | Main Crystal Input                       |
| 30      | XTAL_P                      | Main Crystal Output                      |
| 31      | VDDA                        | Analog Power Supply                      |
| 32      | VDDA                        | Analog Power Supply                      |
| 33      | GND                         | Ground                                   |

**Note:** Based on the schematic analysis, all ESP32-C3 pins are utilized for specific functions within the Hope Badge circuit, and none appear to be readily available as unallocated GPIOs for general user experimentation without modifying the board.

*Information derived from `esp32-c3_datasheet_en.pdf` and `hopebadgeschema-0.8.19.pdf`.*
</details>

## Alternative Firmwares

You can find the original Hacking in Parallel Badge firmware in their [official repository](https://gitlab.com/tidklaas/hip-badge).

You can flash [WLED]. See [WLED Instructions].

You can also flash [MicroPython]. See [MicroPython Instructions].

For both WLED and MicroPython you can't use buttons 1 and 2 due to the USB conflict.

[WLED]: https://kno.wled.ge/
[WLED Instructions]: ./docs/wled.md
[MicroPython]: https://micropython.org/
[MicroPython Instructions]: https://wiki.hope.net/index.php?title=HOPE_XV_Electronic_Badge#MicroPython

## References

*   [HOPE XV Electronic Badge Wiki](https://wiki.hope.net/index.php?title=HOPE_XV_Electronic_Badge)
*   [HOPE XV Electronic Badge User Manual](https://web.archive.org/web/20240806210845/https://wiki.hope.net/images/6/6b/HOPE_XV_Electronic_Badge_User_Manual.pdf)
*   [HOPE XV Badge Schematics](./docs/hopebadgeschema-0.8.19.pdf)
*   [Blinkenlights Pinout](https://gitlab.com/tidklaas/hip-badge/-/blob/master/firmware/blinkenlights/main/Kconfig.projbuild)
*   [Blinkenlights Firmware](https://gitlab.com/tidklaas/hip-badge/-/blob/master/firmware/blinkenlights/main/blinken.c?ref_type=heads)
* Infrared trigger:
  * Encoding: `NECext`
  * Address: `0xD880`
  * Command: `0xDD22`
