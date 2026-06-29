# Hello World

Basic "Hello World" example for Soldered Inkplate 13 using LVGL.

## Overview

Initialises the display and renders a "Hello World!" label centered on the 6-color e-paper screen using the LVGL graphics library.

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

"Hello World!" centered on the display in Montserrat 48 font.

## Notes

- `display.enableDithering(true)` is required for correct color rendering on the 6-color panel.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
