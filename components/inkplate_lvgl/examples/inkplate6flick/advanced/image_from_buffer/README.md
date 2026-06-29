# Image From Buffer

Display an image from a RAM buffer on Soldered Inkplate 6Flick using LVGL.

## Overview

Renders a 600×448 RGB888 (3 bytes/pixel) image stored in `cat.h` using an LVGL image descriptor. Dithering is enabled for best grayscale output on the e-paper panel.

## Hardware Required

- Soldered Inkplate 6Flick
- USB cable

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate6 Flick**

Replace `cat.h` with your own image converted to RGB888 format using the image tool linked below.

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

The cat image displayed centered on the screen.

## Notes

- Image format: RGB888 (3 bytes/pixel), 600×448.
- `display.enableDithering(true)` improves image quality on the e-paper panel.
- `display.frontlight.setState(false)` disables the frontlight at startup.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
