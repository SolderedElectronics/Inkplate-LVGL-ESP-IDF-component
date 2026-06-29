# Frontlight

Interactive frontlight brightness control example for Soldered Inkplate 6Flick.

## Overview

Demonstrates interactive frontlight brightness control via UART0. Send `+` to increase brightness, `-` to decrease it, or `s` to trigger a light show. Current brightness is shown on the display.

## Hardware Required

- Soldered Inkplate 6Flick
- USB cable
- Serial terminal at 115200 baud

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate6 Flick**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

- Display shows current brightness level and instructions.
- Send `+` via serial to increase brightness (max 63).
- Send `-` via serial to decrease brightness (min 0).
- Send `s` via serial to trigger a light show (fade up/down 4 times).
- Display updates with partial refresh on each change.

## Notes

- `display.frontlight.setState(true)` enables the frontlight; `setState(false)` disables it.
- `display.frontlight.setBrightness(value)` sets brightness 0–63.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.partialUpdate()` refreshes only the changed region.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
