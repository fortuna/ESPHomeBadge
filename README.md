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

*   [ ] LED control and effects
*   [ ] IR Transmitter/Receiver

## Getting Started

1.  **Prerequisites:**
    *   A HOPE XV Electronic Badge.
    *   A computer with ESPHome installed. (See the [ESPHome Getting Started Guide](https://esphome.io/guides/getting_started_command_line.html))
    *   USB cable to connect the badge to your computer.
3. **Customize (optional):**
    * Customize the badge to your liking by editing the deployment.yaml configuration file.
2.  **Flashing:**
    *   Connect the HOPE Badge to your computer.
    *   Use the ESPHome command-line tool to compile and upload your configuration to the badge:
        ```bash
        esphome run deployment.yaml
        ```


## Contributing

Contributions are welcome! If you have ideas, bug fixes, or new features, please open an issue or submit a pull request.

## References

*   [HOPE XV Electronic Badge Wiki](https://wiki.hope.net/index.php?title=HOPE_XV_Electronic_Badge)
*   [HOPE XV Electronic Badge User Manual](https://web.archive.org/web/20240806210845/https://wiki.hope.net/images/6/6b/HOPE_XV_Electronic_Badge_User_Manual.pdf)
*   [Blinkenlights Pinout](https://gitlab.com/tidklaas/hip-badge/-/blob/master/firmware/blinkenlights/main/Kconfig.projbuild)
*   [Blinkenlights Firmware](https://gitlab.com/tidklaas/hip-badge/-/blob/master/firmware/blinkenlights/main/blinken.c?ref_type=heads)
* Infrared trigger:
  * Encoding: `NECext`
  * Address: `0xD880`
  * Command: `0xDD22`



