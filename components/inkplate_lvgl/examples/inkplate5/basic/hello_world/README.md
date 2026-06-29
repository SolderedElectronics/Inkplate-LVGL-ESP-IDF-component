# Hello World

Display "Hello World!" on Soldered Inkplate 5 using LVGL.

## Overview

Demonstrates the most basic usage of the Inkplate 5 with LVGL by initializing the display and rendering a "Hello World!" label on the e-paper screen using the LVGL graphics library.

## Hardware Required

- Soldered Inkplate 5
- USB cable

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate5**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

The text "Hello World!" centered on the Inkplate 5 screen.

## Notes

- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
