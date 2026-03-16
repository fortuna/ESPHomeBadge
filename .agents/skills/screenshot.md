# Screenshot Skill

Take screenshots of the ESPHome badge display simulator (SDL/host build).

## Overview

This skill builds the firmware for the host/SDL platform using ESPHome and captures
screenshots of the display output. Screenshots are saved to `docs/screenshots/`.

## How to Take Screenshots

### Prerequisites

The host build requires:
- ESPHome installed (available via `pip install esphome` or the project's dev environment)
- SDL2 libraries (`libsdl2-dev`)
- A display environment or virtual framebuffer (`Xvfb`) for headless operation

Check availability:
```bash
which esphome && esphome version
dpkg -l libsdl2-dev 2>/dev/null | grep -q ii && echo "SDL2 installed"
```

### Build the SDL Simulator

From the repo root:
```bash
cd firmware_display
esphome compile main_host.yaml
```

The compiled binary will be at `.esphome/build/sdl/.pioenvs/sdl/program`.

### Capture Screenshots

The SDL display supports programmatic screenshot capture. Run the simulator with a
virtual framebuffer and use `scrot`, `import` (ImageMagick), or `ffmpeg` to capture:

```bash
# Start virtual display
Xvfb :99 -screen 0 1024x768x24 &
export DISPLAY=:99

# Run simulator (it opens a 320x240 SDL window)
cd firmware_display
.esphome/build/sdl/.pioenvs/sdl/program &
SIM_PID=$!

# Wait for display to render, then capture
sleep 3
import -window root -crop 320x240+0+0 ../docs/screenshots/page_badge.png

kill $SIM_PID
```

### Navigating Pages

The simulator uses keyboard keys mapped to badge buttons:
- `m` → button_1 (menu/select)
- `,` → button_2
- `.` → button_3
- `/` → button_4
- `u` → key_up
- `d` → key_down
- `[` → key_left
- `]` → key_right

Use `xdotool` to send keypresses to navigate between pages and capture each one:
```bash
xdotool key m        # press menu button
sleep 0.5
xdotool key d        # press down
sleep 0.5
import -window root -crop 320x240+0+0 ../docs/screenshots/page_air.png
```

### Screenshot Pages

The badge has 4 display pages (apps):
1. **page_badge** – Badge identity (title, subtitle)
2. **page_air** – Air quality (eCO2, TVOC values)
3. **page_qr** – QR code display
4. **page_ir_blast** – IR blast control

Screenshots are stored in `docs/screenshots/`:
- `docs/screenshots/page_badge.png`
- `docs/screenshots/page_air.png`
- `docs/screenshots/page_qr.png`
- `docs/screenshots/page_ir_blast.png`

### Mock Sensor Values

The host build (`main_host.yaml`) sets mock values on boot:
- Badge title: `"Title"`
- Badge subtitle: `"Subtitle"`
- eCO2: `4321.0 ppm`
- TVOC: `123.0 ppb`
- QR code: `https://example.com`

To change mock values, edit `firmware_display/main_host.yaml` under `on_boot`.

## Troubleshooting

- **No display**: Make sure `DISPLAY` is set or Xvfb is running.
- **SDL init error**: Install `libsdl2-2.0-0` (runtime) and `libsdl2-dev` (headers).
- **Build fails**: Run `esphome compile main_host.yaml` and check the error output.
- **Black screenshot**: Increase the sleep time after starting the simulator to allow rendering.
