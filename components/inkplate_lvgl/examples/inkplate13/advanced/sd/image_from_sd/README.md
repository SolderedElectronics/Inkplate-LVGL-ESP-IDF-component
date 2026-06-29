# Image From SD Card

Display JPEG images from an SD card on Soldered Inkplate 13 using LVGL.

## Overview

Initialises the SD card, then uses the LVGL image widget to decode and display JPEG files stored on the card. Displays `picture1.jpg` for 5 seconds, then switches to `picture2.jpg` for 5 seconds.

## Hardware Required

- Soldered Inkplate 13
- USB cable
- microSD card with `picture1.jpg` and `picture2.jpg` in the root directory (FAT32 formatted)

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate13**

Copy `picture1.jpg` and `picture2.jpg` to the root of a FAT32-formatted microSD card and insert it into the Inkplate 13.

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

- `picture1.jpg` shown for 5 s.
- `picture2.jpg` shown for 5 s.

## Notes

- SD filesystem path prefix is `S:/` (e.g. `"S:/picture1.jpg"`).
- `display.enableDithering(true)` improves image quality on the 6-color panel.
- `display.sdCardSleep()` powers down the SD card after use.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
