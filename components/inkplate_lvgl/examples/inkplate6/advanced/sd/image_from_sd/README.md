# Image From SD Card

Display a JPEG image from a microSD card on Soldered Inkplate 6 using LVGL.

## Overview

Initializes the SD card, loads `cat.jpg` from the root directory using the LVGL SD filesystem driver (registered automatically by the Inkplate constructor), displays it, then puts the SD card to sleep for power saving.

## Hardware Required

- Soldered Inkplate 6
- USB cable
- microSD card with `cat.jpg` in root

## Setup

1. Copy `cat.jpg` to the root of a FAT-formatted microSD card.
2. Insert the SD card into Inkplate 6.

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate6**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

The cat image is loaded from the SD card and shown on the display.

## Notes

- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.
- The LVGL SD filesystem driver is registered automatically by the Inkplate constructor.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
