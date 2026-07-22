# Color Palette

Demonstrate the 6 native colors of the Soldered Inkplate 7 e-paper display using LVGL.

## Overview

Renders six vertical rectangles side by side, one per supported color (black, white, yellow, red, blue, green), using LVGL, then calls `display()` to push the framebuffer to the screen.

## Hardware Required

- Soldered Inkplate 7
- USB cable

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate7**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

Six vertical rectangles: black, white, yellow, red, blue, green.

## Notes

- `display.enableDithering(true)` is required for correct color rendering on the 6-color panel.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
