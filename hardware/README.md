# Hardware Definitions

This directory houses the hardware definition files for various badge versions.
These files are essential starting points for developing custom firmware for your specific badge.

## Official Hardware Specifications

The definitive source for the board hardware specifications can be found at:
https://gitlab.com/tidklaas/hip-badge/-/tree/master/hardware

> **Note:** When browsing that repository, ensure you select the correct Git tag corresponding to your badge's version (e.g., `REL_0.8.13`, `REL_0.8.15`). This will ensure you are viewing the right specifications for your physical hardware.

## File Naming Convention

The hardware definition files in this directory follow a consistent naming pattern:

`badge_${version}.yaml`

Where `${version}` represents the hardware version of the badge (e.g., `0.8.13`). You can find the hardware version on the back of the PCB board (You need to open the enclosure).

For example, `badge_0.8.13.yaml` would be the hardware definition for the version 0.8.13 of the badge.

## How to Use These Definitions

These YAML files are designed to be included in your ESPHome firmware configurations. You can use them as a base and extend them with your custom components and logic.

**Example**

```yaml
packages:
  # Include the base hardware definition
  hardware: !include badge_0.8.15.yaml

# ... your custom firmware configuration below
```
