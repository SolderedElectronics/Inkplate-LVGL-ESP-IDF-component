# Image From Buffer

Display an RGB888 image stored in a C array on Soldered Inkplate 6Color using LVGL.

## Overview

Displays a JPG image stored as a C array (`cat.h`) directly from flash memory using LVGL's image widget with RGB888 format. Dithering is enabled for best color reproduction on the 7-color e-paper panel.

## Hardware Required

- Soldered Inkplate 6Color
- USB cable

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate6 Color**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

Cat image appears centered on the display.

## Notes

- `cat.h` was generated with https://notisrac.github.io/FileToCArray/ (options: Hex(0x00), 24bit RGB, Multi line, Separate bytes of pixels).
- Dithering is only supported in FULL render mode.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
