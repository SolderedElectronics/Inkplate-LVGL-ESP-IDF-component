# Deep Sleep with Button Wake

Wake Soldered Inkplate 6 from deep sleep via timer or button press using LVGL.

## Overview

Wakes from deep sleep either on timer (30 s) or when the WAKE button (GPIO 36) is pressed (active-low). The wake reason and boot count are shown on the display.

## Hardware Required

- Soldered Inkplate 6
- USB cable

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate6**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

Display shows boot count and wake reason, then enters deep sleep. Press the WAKE button or wait 30 s to wake again.

## Notes

- GPIO 36 is the WAKE button pin (active-low).
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
