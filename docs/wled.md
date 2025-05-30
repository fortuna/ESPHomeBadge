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