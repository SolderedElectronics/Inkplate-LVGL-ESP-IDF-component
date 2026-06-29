# Image From Buffer

Display an image from a RAM buffer on Soldered Inkplate 10 using LVGL.

## Overview

Renders a 1200×820 L8 (grayscale, 1 byte/pixel) image stored in `cat.h` using an LVGL image descriptor. Dithering is enabled for best output on the e-paper panel.

## Hardware Required

- Soldered Inkplate 10
- USB cable

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate10**

Replace `cat.h` with your own image converted to L8 grayscale format using the image tool linked below.

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

The cat image displayed centered on the screen.

## Notes

- Image format: L8 (grayscale, 1 byte/pixel), 1200×820.
- `display.enableDithering(true)` improves image quality on the 3-bit grayscale panel.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
