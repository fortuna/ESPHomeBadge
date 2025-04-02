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
*   [x] Tactile Buttons
*   [x] LED strip
*   [x] IR receiver
*   [x] IR transmitter
*   [ ] Vibration

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



