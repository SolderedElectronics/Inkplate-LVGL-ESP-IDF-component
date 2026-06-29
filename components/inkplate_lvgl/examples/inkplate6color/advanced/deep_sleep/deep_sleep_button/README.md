# Deep Sleep with Button Wake

Wake Soldered Inkplate 6Color from deep sleep via timer or button press using LVGL.

## Overview

Demonstrates waking the ESP32 from deep sleep via two sources: a hardware button on GPIO36 (EXT0 wakeup) or a 30-second timer. The boot counter and wakeup reason are displayed on screen.

## Hardware Required

- Soldered Inkplate 6Color
- USB cable

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate6 Color**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

Display shows boot count and wakeup reason, then sleeps. Press the WAKEUP button (GPIO36) or wait 30 s to wake again.

## Notes

- GPIO 36 is the WAKEUP button pin (active-low).
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
