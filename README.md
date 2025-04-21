# ESPHomeBadge

[![Status: Work in Progress](https://img.shields.io/badge/Status-Work%20in%20Progress-yellow)](https://img.shields.io/badge/Status-Work%20in%20Progress-yellow)

> **Note:** This project is currently under active development. Expect changes and potential issues.

This repository provides a way to program the [HOPE XV Electronic Badge](https://wiki.hope.net/index.php?title=HOPE_XV_Electronic_Badge) ([archive](https://web.archive.org/web/20241214102029/https://wiki.hope.net/index.php?title=HOPE_XV_Electronic_Badge)) using [ESPHome](https://esphome.io/). ESPHome is a powerful yet simple framework for controlling your ESP32-based devices.

By [Vinicius Fortuna](https://www.viniciusfortuna.com).

## Why ESPHome for the HOPE Badge?

By enabling users to program the badge with ESPHome, this project aims to:

*   **Simplify Development:** ESPHome's YAML-based configuration and built-in components make it easier to get started with custom functionality.
*   **Increase Accessibility:** Lower the barrier to entry for hacking and customizing the HOPE Badge.
*   **Foster Community:** Encourage collaboration and sharing of custom badge configurations.
* **Leverage Home Assistant:** Integrate the badge into your Home Assistant setup.

## Features (Planned/In Development)

Hardware Definition
*   [x] [Tactile Buttons](https://esphome.io/components/binary_sensor/) (GPIO19, GPIO18, GPIO10, GPIO09)
*   [x] [Green LED](https://esphome.io/components/output/ledc.html) (GPIO13)
*   [x] [RGB LED strip](https://esphome.io/components/light/esp32_rmt_led_strip) (GPIO08)
*   [x] [IR receiver](https://esphome.io/components/remote_receiver.html) (GPIO03)
*   [x] [IR transmitter](https://esphome.io/components/remote_transmitter.html) (GPIO02)
*   [x] Vibration motor (GPIO13/SPIWP)
*   [x] [SGP30 Air Quality Sensor](https://esphome.io/components/sensor/sgp30.html) (I2C, U7)
*   [x] [MAX17048 Fuel Gauge](https://esphome.io/components/sensor/max17043.html) (I2C, U4)
*   [ ] [PCF8574 GPIO Expander](https://esphome.io/components/pcf8574.html) (I2C, U9)

Remote Access
*   [x] [Wifi provisioning](https://esphome.io/components/wifi.html)
*   [x] [Web Server](https://esphome.io/components/web_server.html) controller
*   [x] [OTA update](https://esphome.io/components/ota/) support

Application
*   [ ] LED control and effects
*   [ ] IR Transmitter/Receiver

## Getting Started

1.  **Prerequisites:**
    *   A HOPE XV Electronic Badge.
    *   A computer with ESPHome installed. (See the [ESPHome Getting Started Guide](https://esphome.io/guides/getting_started_command_line.html))
    *   USB cable to connect the badge to your computer.
2.  **Customization (Optional):**
    * Customize the badge to your liking by editing the deployment.yaml configuration file.
3.  **Flashing:**
    *   Connect the HOPE Badge to your computer.
    *   Use the ESPHome command-line tool to compile and upload your configuration to the badge:
        ```bash
        esphome run deployment.yaml
        ```

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

This will enter flashing mode and prevent the firmware from running and breaking the USB connection.

Once flashing is complete, you won't see any logs, because as soon as the firmware starts, it breaks USB.

Also, the firmware won't run correctly somehow. You need to reset it (turn it off and back on).

If you don't disconnect the USB cable, you will see that buttons 1 and 2 will not work. You must disconnect the cable.

This is the full recipe for successfully flashing:
1. Turn off the badge
1. Connect the badge via USB
1. Turn it on in flash mode
1. Flash
1. Disconnect from USB
1. Turn it off and back on

## Contributing

Contributions are welcome! If you have ideas, bug fixes, or new features, please open an issue or submit a pull request.


## ESP32-C3 Pin Usage on Hope Badge

Here is a table summarizing the ESP32-C3 pins based on the ESP32-C3 datasheet and the badge schematic. The "Usage in Badge" column details how the pin is connected in the schematic, and "Availability" indicates if the pin is easily accessible for general use on this specific badge design.

| Pin No. | Pin Name (Datasheet)        | Usage in Badge (Schematic)                         | Availability |
| :------ | :-------------------------- | :------------------------------------------------- | :----------- |
| 1       | LNA_IN                      | RF Antenna Input                         | Used         |
| 2       | VDD3P3                      | +3.3V Power Supply                       | Used (Power) |
| 3       | VDD3P3                      | +3.3V Power Supply                       | Used (Power) |
| 4       | XTAL_32K_P / GPIO0          | 32kHz Crystal Input                      | Used         |
| 5       | XTAL_32K_N / GPIO1          | 32kHz Crystal Output                     | Used         |
| 6       | GPIO2                       | IR Transmitter Control                   | Used         |
| 7       | CHIP_EN                     | Chip Enable Control                      | Used         |
| 8       | GPIO3                       | IR Receiver Input                        | Used         |
| 9       | MTMS / GPIO4                | SPI Chip Select 1 (SPI_CS1)              | Used         |
| 10      | MTDI / GPIO5                | SPI Data In (SPI_SDI/MOSI)               | Used         |
| 11      | VDD3P3_RTC                  | +3.3V RTC Power Supply                   | Used (Power) |
| 12      | MTCK / GPIO6                | SPI Clock (SPI_SCK)                      | Used         |
| 13      | MTDO / GPIO7                | TFT Display Data/Command (TFT_DC)        | Used         |
| 14      | GPIO8                       | RGB LED Data (RGB_DAT)                   | Used         |
| 15      | GPIO9 / BOOT                | Tactile Switch B Input (TACT_B)          | Used         |
| 16      | GPIO10                      | Tactile Switch A Input (TACT_A)          | Used         |
| 17      | VDD3P3_CPU                  | +3.3V CPU Power Supply                   | Used (Power) |
| 18      | VDD_SPI / GPIO11            | SPI Flash Power Supply              | Used (Power) |
| 19      | SPIHD / GPIO12              | LED Control (D28)                   | Used         |
| 20      | SPIWP / GPIO13              | Vibration Motor Control        | Used         |
| 21      | SPICS0 / GPIO14             | Internal Flash Chip Select               | Used         |
| 22      | SPICLK / GPIO15             | Internal Flash Clock                     | Used         |
| 23      | SPID / GPIO16               | Internal Flash Data Out (MOSI)           | Used         |
| 24      | SPIQ / GPIO17               | Internal Flash Data In (MISO)            | Used         |
| 25      | GPIO18 / USB_D-             | USB Data Negative (USB-)                 | Used         |
| 26      | GPIO19 / USB_D+             | USB Data Positive (USB+)                 | Used         |
| 27      | U0RXD / GPIO20              | UART0 Receive (U0RXD)                    | Used         |
| 28      | U0TXD / GPIO21              | UART0 Transmit (U0TXD)                   | Used         |
| 29      | XTAL_N                      | Main Crystal Input                       | Used         |
| 30      | XTAL_P                      | Main Crystal Output                      | Used         |
| 31      | VDDA                        | Analog Power Supply                      | Used (Power) |
| 32      | VDDA                        | Analog Power Supply                      | Used (Power) |
| 33      | GND                         | Ground                                   | Used (Power) |

**Note:** Based on the schematic analysis, all ESP32-C3 pins are utilized for specific functions within the Hope Badge circuit, and none appear to be readily available as unallocated GPIOs for general user experimentation without modifying the board.

*Information derived from `esp32-c3_datasheet_en.pdf` and `hopebadgeschema-0.8.19.pdf`.*

## WLED Instructions

If you're more interested in LED effects, you can flash the badge with [WLED](https://kno.wled.ge/) instead ([effects list](https://kno.wled.ge/features/effects/)). Here's how:


### Flash the WLED firmware
1. Plug the badge with a USB cable.
1. Use the [web installer](https://install.wled.me/) to flash the firmware.
1. In the process, make sure to configure the wifi.

### Configure the LED strip
1. Open the web controller
1. Go to _Config_ -> _LED Preferences_
1. Under _Hardware Setup_ -> _LED outputs_:
    - **Type:** WS281x
    - **Color order:** GRB
    - **Length:** 16
    - **Data GPIO:** 8

Now you are ready to use the badge with the web UI!

### Configure Button Actions

You can configure the two rightmost buttons to trigger actions in WLED.

> Note: the first two buttons (GPIO 18 and 19) cannot be configured with WLED because they conflict with the USB connection.

1. Go to _Config_ -> _LED Preferences_
1. In the buttons section:
    - **Button 2:** GPIO 10, Pushbutton
    - **Button 3:** GPIO 9, Pushbutton
1. In the Home page, create some presets. [Presets](https://kno.wled.ge/features/presets/) can be used to issue commands (e.g. toggle, change brightness) or select a specific effect.
1. Go to _Config_ -> _Time & Macros_
    - Under _button actions_, map the configured buttons to the desired Preset IDs.

### Configure IR remote (optional)

If you want to control WLED with an IR remote:

1. Go to _Config_ -> _LED Preferences_
1. In the IR section:
    - **IR GPIO:** 3
    - **Type:** _JSON Remote_
    - Follow the [documentation](https://kno.wled.ge/interfaces/json-ir/json_infrared/) to configure the remote commands.

Now you're all set to control the badge's LED effects using the web UI, buttons, or an IR remote!

### Configure 2D LED Mapping

<img src=https://github.com/user-attachments/assets/20c955c5-9e6a-46d4-998e-e81c119c059b align=right width=250>
If you want to use 2D effects, you should map the LEDs so the effects show up correctly.

Follow the [official mapping documention'](https://kno.wled.ge/advanced/mapping/) on how to create the `ledmap.json` file, which should have the content below:

```json
{
  "map": [
    14, 15,  0,  1, 2, 
    13, -1, -1, -1, 3,
    12, -1, -1, -1, 4,
    11, -1, -1, -1, 5,
    10,  9,  8,  7, 6
  ],
  "width":  5,
  "height": 5
}
```

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



