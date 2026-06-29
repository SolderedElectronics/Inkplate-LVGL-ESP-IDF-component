# Color Palette

Display the three native colors of Inkplate 2 on Soldered Inkplate 2 using LVGL.

## Overview

Demonstrates the three native colors of the Inkplate 2 e-paper display (black, white, red) by rendering three vertical rectangles side by side using the LVGL graphics library.

## Hardware Required

- Soldered Inkplate 2
- USB cable

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate2**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

Three vertical rectangles: black, white, and red.

## Notes

- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
