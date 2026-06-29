# Black & White

Demonstrate 1-bit black & white mode on Soldered Inkplate 6 using LVGL.

## Overview

Demonstrates 1-bit (black & white) mode on Inkplate 6 by splitting the display into a black left half and a white right half using LVGL.

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

Screen split vertically: left half black, right half white.

## Notes

- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
