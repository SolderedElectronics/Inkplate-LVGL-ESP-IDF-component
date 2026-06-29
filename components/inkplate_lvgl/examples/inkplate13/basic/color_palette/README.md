# Color Palette

Color palette example for Soldered Inkplate 13 using LVGL.

## Overview

Demonstrates the 6 native colors of the Inkplate 13 e-paper display by rendering six vertical rectangles side by side using LVGL: black, white, green, yellow, blue, and red.

## Hardware Required

- Soldered Inkplate 13
- USB cable

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate13**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

Six vertical rectangles spanning the full display: black, white, green, yellow, blue, red.

## Notes

- `display.enableDithering(true)` is required for correct color rendering on the 6-color panel.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
