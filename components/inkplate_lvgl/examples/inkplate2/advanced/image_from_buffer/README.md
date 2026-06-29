# Image From Buffer

Display an RGB565 image stored in a C array on Soldered Inkplate 2 using LVGL.

## Overview

Demonstrates how to display an RGB565 image stored in a C array on the Inkplate 2 e-paper display using LVGL's image widget. Dithering is enabled to improve the visual quality of the image on the 3-color (black, white, red) e-paper panel.

## Hardware Required

- Soldered Inkplate 2
- USB cable

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate2**

To use a custom image, replace `image_ex.h` with your own converted array (see image tool below).

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

Display shows the image from `image_ex.h` centered on screen.

## Notes

- `image_ex.h` contains RGB565 pixel data at 212×104 resolution.
- To use a custom image, replace `image_ex.h` with your own converted array.
- Dithering is only supported in FULL render mode.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
