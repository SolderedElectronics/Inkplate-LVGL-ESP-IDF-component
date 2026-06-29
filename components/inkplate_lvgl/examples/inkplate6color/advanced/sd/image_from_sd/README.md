# Image From SD Card

Display a JPEG image from a microSD card on Soldered Inkplate 6Color using LVGL.

## Overview

Initializes the SD card, then uses LVGL's image widget to decode and display a JPEG file (`cat.jpg`) stored on the SD card. Dithering is enabled for best color reproduction. The SD card is put to sleep after use to save power.

## Hardware Required

- Soldered Inkplate 6Color
- USB cable
- microSD card with `cat.jpg` in root

## Setup

1. Copy `cat.jpg` to the root of a FAT32-formatted microSD card.
2. Insert the card into Inkplate 6Color.

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

- Dithering is enabled for best color reproduction on the 7-color panel.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
