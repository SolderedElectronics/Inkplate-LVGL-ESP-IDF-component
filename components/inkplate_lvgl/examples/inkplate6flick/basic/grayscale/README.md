# Grayscale

Grayscale bars example for Soldered Inkplate 6Flick using LVGL.

## Overview

Displays 8 vertical bars showing 8 shades of gray (0–7) using LVGL objects in 3-bit grayscale display mode.

## Hardware Required

- Soldered Inkplate 6Flick
- USB cable

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate6 Flick**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

Screen shows 8 vertical gray bars from black (0) to white (7), each labeled with its shade number.

## Notes

- `display.frontlight.setState(false)` disables the frontlight at startup.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
