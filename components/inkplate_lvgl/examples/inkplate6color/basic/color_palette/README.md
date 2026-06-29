# Color Palette

Display all 7 native colors of Inkplate 6Color on Soldered Inkplate 6Color using LVGL.

## Overview

Demonstrates all 7 native colors of the Inkplate 6Color e-paper display (black, white, green, yellow, blue, red, orange) by rendering seven vertical rectangles side by side using LVGL.

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

Seven vertical rectangles: black, white, green, yellow, blue, red, orange.

## Notes

- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
