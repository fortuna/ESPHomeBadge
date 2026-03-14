# AGENTS.md — AI Assistant Guide for ESPHomeBadge

This document provides context, conventions, and workflows for AI assistants working on the ESPHomeBadge project.

---

## Project Overview

**ESPHomeBadge** is a firmware framework for the [HOPE XV Electronic Badge](https://wiki.hope.net/index.php?title=HOPE_XV_Electronic_Badge), an ESP32-C3-based conference badge. It uses [ESPHome](https://esphome.io/) YAML configuration to make the badge programmable without writing raw C/MicroPython, and enables integration with Home Assistant.

**Repository:** https://github.com/fortuna/ESPHomeBadge
**License:** Apache 2.0
**Status:** Active development (Work in Progress)

---

## Repository Structure

```
ESPHomeBadge/
├── .github/workflows/test.yaml   # CI: lint + compile checks
├── .vscode/tasks.json            # VS Code build tasks
├── .yamllint                     # YAML linting rules
├── docs/                         # Hardware schematics (PDFs) + WLED guide
├── hardware/                     # Badge hardware definitions per version
│   ├── README.md                 # Explains naming convention
│   ├── badge_0.8.13.yaml         # Base hardware definition
│   ├── badge_0.8.15.yaml         # Extends 0.8.13
│   └── badge_0.8.20.yaml         # Latest: PCF8574 expander, SGP40, NFC
├── firmware_display/             # Firmware for badges WITH a screen
│   ├── main.yaml                 # Production firmware
│   ├── main_dev.yaml             # Dev build (OTA enabled, USB buttons removed)
│   ├── main_host.yaml            # PC/SDL host for UI prototyping
│   ├── lvgl.yaml                 # LVGL UI framework config + styles
│   ├── page_selector.yaml        # App switcher overlay
│   ├── menu.h                    # C++ helpers for LVGL group management
│   ├── secrets.yaml              # Template (do not commit real values)
│   └── apps/
│       ├── page_air.yaml         # Air quality sensor display
│       ├── page_badge.yaml       # Badge info / identity
│       ├── page_ir_blast.yaml    # IR transmitter interface
│       ├── page_qr.yaml          # QR code generator
│       └── page_wifi.yaml        # WiFi configuration (WIP)
├── firmware_nodisplay/           # Firmware for badges WITHOUT a screen
│   ├── main.yaml
│   ├── main_dev.yaml
│   └── secrets.yaml
├── logbook.md                    # Developer notes and design decisions
└── README.md                     # Main project documentation
```

---

## Hardware

### Microcontroller

- **Chip:** ESP32-C3 (single-core 32-bit RISC-V, no PSRAM)
- **Base board:** esp32-c3-devkitm-1
- **Flash mode:** DIO (not QIO) — this frees GPIO12/13 for vibration motor use

### Key Components

| Component | Interface | Notes |
|-----------|-----------|-------|
| ST7789V TFT LCD (320×240) | SPI | DC: GPIO00, CS: GPIO01/04 |
| WS2812B LED strip (16 LEDs) | GPIO08 | RMT, 48 symbols |
| IR Receiver (IRM-H638) | GPIO03 | RMT, 86 symbols |
| IR Transmitter (VSMY1850) | GPIO02 | RMT, 48 symbols |
| Button 1 (SW3) | GPIO19 | **USB D+ conflict** |
| Button 2 (SW4) | GPIO18 | **USB D- conflict** |
| Button 3 (SW1) | GPIO10 | |
| Button 4 (SW2) | GPIO09 | BOOT pin |
| Green LED (D28) | GPIO13 | |
| Vibration Motor | GPIO12/13 | Version dependent |
| MAX17048 Battery Sensor | I2C 0x36 | |
| SGP30 Air Quality (v0.8.13–0.8.15) | I2C 0x58 | eCO2 + TVOC |
| SGP40 Air Quality (v0.8.20) | I2C 0x59 | VOC Index |
| PCF8574 GPIO Expander (v0.8.20) | I2C 0x20 | Manages buttons 1 & 2 |
| NFC Tag ST25DV / NT3H2111 (v0.8.20) | I2C 0x55 | Software pending |

### USB/Button Conflict — Critical Note

Buttons 1 (GPIO19) and 2 (GPIO18) share pins with USB data lines. While the USB cable is connected, these buttons do not function. Firmware must be flashed by holding Button 4 (BOOT) on power-up to enter flash mode.

Development firmware (`main_dev.yaml`) omits buttons 1 and 2 to avoid conflicts during serial monitoring.

### Hardware Versions

- **v0.8.13** — Original, base hardware definition
- **v0.8.15** — Extends v0.8.13 (slight changes, still uses SGP30)
- **v0.8.20** — Latest: PCF8574 GPIO expander, SGP40, NFC tag, all GPIOs fully utilized

---

## ESPHome Architecture

### Layered Package System

Configuration is modular using ESPHome's `!include` and `!extend`:

1. **Hardware layer** (`hardware/badge_X.X.X.yaml`) — pure hardware definitions, no app logic. Exposes standardized component IDs used by all firmware variants.
2. **Firmware layer** (`firmware_*/main.yaml`) — WiFi, web server, sensors, LED effects, app logic.
3. **UI layer** (`lvgl.yaml`, `apps/page_*.yaml`) — display, styles, page definitions.
4. **Dev overrides** (`main_dev.yaml`) — extends the production config with dev-time settings.

### Standard Component IDs

When the hardware YAML exposes components, always use these canonical IDs:

| ID | Type | Description |
|----|------|-------------|
| `button_1` … `button_4` | `binary_sensor` | Tactile buttons |
| `led_strip` | `light` | WS2812B 16-LED strip |
| `board_led` | `light` | Green onboard LED |
| `ir_receiver` | `remote_receiver` | IR receiver |
| `ir_transmitter` | `remote_transmitter` | IR transmitter |
| `battery_sensor` | `sensor` | Battery % via MAX17048 |
| `air_sensor` | `sensor` | Air quality (SGP30/40) |
| `badge_display` | `display` | ST7789V LCD |
| `vibration_motor` | `output` | Vibration motor |
| `badge_i2c` | `i2c` | Shared I2C bus |

### Sensor Abstraction Pattern

Air quality data is exposed via scripts to decouple sensor hardware from UI:

- `badge_on_eco2_value` — triggered with eCO2 value
- `badge_on_tvoc_value` — triggered with TVOC value

App pages hook into these scripts, not directly into the sensor component. This allows SGP30 vs SGP40 substitution across hardware versions.

### LVGL UI

- **Buffer size:** 25% of RAM (PSRAM unavailable on ESP32-C3)
- **Color depth:** 8-bit palette mode
- **Page structure:** Each app is a full-screen LVGL page; `page_selector.yaml` provides the switcher overlay
- **Focus groups:** Each page defines its own LVGL group; `menu.h` C++ helpers manage group assignment and navigation

`menu.h` provides:
- `recursive_set_group(obj, group)` — add widget tree to LVGL group
- `find_indev_by_driver(drv)` — locate input devices
- `get_group_item_by_index(group, idx)` — access group objects by index
- `get_focused_item_index_in_group(group)` — get current focus index

### RMT Symbol Allocation

ESP32-C3 has 192 total RMT symbols. Current allocation:

| Peripheral | Symbols |
|-----------|---------|
| LED strip | 48 |
| IR TX | 48 |
| IR RX | 86 |

Do not exceed these limits. Changing them requires re-verifying that all three peripherals still function simultaneously.

---

## Development Workflows

### Prerequisites

- Python 3.x
- ESPHome: `pip install esphome`
- (Optional) VS Code with the tasks in `.vscode/tasks.json`

### Secrets File

Each firmware directory has a `secrets.yaml` template. Copy and fill in values before building:

```yaml
wifi_ssid: "YourNetwork"
wifi_password: "YourPassword"
ap_password: "fallback_ap_password"
ota_password: "your_ota_password"
```

**Never commit real credentials.** The CI uses generated dummy secrets.

### Building

```bash
# Compile (no flash)
esphome compile firmware_display/main_dev.yaml

# Compile and flash over USB
esphome upload firmware_display/main_dev.yaml

# Use build cache for faster rebuilds (recommended)
PLATFORMIO_BUILD_CACHE_DIR=/tmp/esphome esphome compile ...
```

### Recommended Firmware for Development

Use `main_dev.yaml` (in either `firmware_display/` or `firmware_nodisplay/`) during active development. It:
- Enables OTA updates (flash over WiFi after first USB flash)
- Uses static device naming (no MAC suffix)
- Removes buttons 1 & 2 to prevent USB conflicts during serial monitoring
- Reads WiFi credentials from `secrets.yaml`

### Host/Simulator Build

`firmware_display/main_host.yaml` compiles a native Linux binary using SDL for the display. Use this to prototype UI changes without hardware.

### Flashing Over USB (with button conflict)

1. Hold **Button 4** (BOOT, GPIO09)
2. Connect USB cable (or press RESET while holding Button 4)
3. Run `esphome upload firmware_display/main_dev.yaml`
4. After flash completes, press RESET or reconnect power

---

## CI/CD

The GitHub Actions workflow (`.github/workflows/test.yaml`) runs for pushes to `main` and for PRs, but only when YAML files are changed:

1. `yamllint` — validates all `*.yaml` files
2. ESPHome compile — `firmware_display/main_dev.yaml` and `firmware_nodisplay/main_dev.yaml`

**A PR is not mergeable if CI fails.** Always verify YAML syntax and that both firmware variants compile before pushing.

---

## Coding Conventions

### YAML

- **Indentation:** 2 spaces (enforced by `.yamllint`)
- **Copyright header** (required at top of every file):
  ```yaml
  # Copyright 2025 Vinicius Fortuna
  # SPDX-License-Identifier: Apache-2.0
  ```
- **Component IDs:** `snake_case`
- **Global flags:** `ALL_CAPS` (e.g., `enable_ir_rx`)
- **Comments:** Explain non-obvious pin choices, hardware constraints, and workarounds. Link to datasheets and ESPHome docs where relevant.
- **Max 1 blank line** between sections

### C++ (menu.h)

- Use `#pragma once` header guard
- Document all functions (describe purpose, parameters, return value)
- NULL-check pointers before use
- Follow LVGL internal API conventions

### File Naming

- Hardware definitions: `badge_X.X.X.yaml` (matches badge PCB version)
- App pages: `page_<name>.yaml`
- Production firmware entry: `main.yaml`
- Dev firmware entry: `main_dev.yaml`
- Host/simulator: `main_host.yaml`

---

## Adding a New App Page

1. Create `firmware_display/apps/page_<name>.yaml`
2. Define an LVGL page with a unique ID (e.g., `page_myapp`)
3. Create a focus group for the page's interactive widgets
4. Register the page in `firmware_display/page_selector.yaml`
5. Include the package in `firmware_display/main.yaml` under `packages:`

App pages should use existing sensor/script interfaces (e.g., `badge_on_eco2_value`) rather than direct component references, to remain compatible across hardware versions.

---

## Adding a New Hardware Version

1. Create `hardware/badge_X.X.X.yaml`
2. Define all components using the standard component IDs listed above
3. Update the `hardware/README.md` with the new version entry
4. Reference the new file in the firmware `main.yaml` and `main_dev.yaml` packages

---

## Known Limitations & Gotchas

| Issue | Details |
|-------|---------|
| **Buttons 1 & 2 don't work over USB** | GPIO18/19 are shared with USB. Disconnect USB to use them in production firmware. |
| **LVGL buffer constraint** | No PSRAM → limited to 25% RAM, 8-bit color palette. |
| **RMT symbol limit** | 192 total; current allocation leaves no headroom. |
| **ESP32-C3 all GPIOs used** | In v0.8.20, every available GPIO is assigned. New hardware features require expanders. |
| **WiFi page is WIP** | `page_wifi.yaml` is partially implemented. |
| **NFC software pending** | v0.8.20 hardware supports NFC but no ESPHome component exists yet. |

---

## Key Files for Context

When working on this project, the most important files to read first are:

1. `README.md` — hardware pin table, flashing instructions, project overview
2. `hardware/badge_0.8.20.yaml` — latest complete hardware definition
3. `firmware_display/main.yaml` — production firmware structure
4. `firmware_display/lvgl.yaml` — display/UI configuration
5. `logbook.md` — design decisions and architectural rationale
6. `.github/workflows/test.yaml` — CI requirements
