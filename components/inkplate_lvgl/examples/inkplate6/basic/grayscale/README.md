# Grayscale

Draw 8-level grayscale bars on Soldered Inkplate 6 using LVGL.

## Overview

Demonstrates 3-bit (8-level grayscale) mode on Inkplate 6 by drawing 8 vertical bars with shades from black (0) to white (7).

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

8 vertical bars from black (leftmost) to white (rightmost), each labeled 0–7.

## Notes

- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.
- Dithering is only supported in FULL render mode.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
