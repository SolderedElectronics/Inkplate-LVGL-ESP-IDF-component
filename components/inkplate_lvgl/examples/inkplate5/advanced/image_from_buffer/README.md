# Image From Buffer

Display an L8 grayscale image stored in a C array on Soldered Inkplate 5 using LVGL.

## Overview

Renders a 1280×720 L8 (grayscale, 1 byte/pixel) image stored in `cat.h` using an LVGL image descriptor. Dithering is enabled for best output on the black-and-white e-paper panel.

## Hardware Required

- Soldered Inkplate 5
- USB cable

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate5**

Replace `cat.h` with the actual image (see `cat.h` for instructions).

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

The cat image is shown on the display.

## Notes

- `cat.h` contains L8 grayscale pixel data at 1280×720 resolution.
- To use a custom image, replace `cat.h` with your own converted array.
- Dithering is only supported in FULL render mode.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
