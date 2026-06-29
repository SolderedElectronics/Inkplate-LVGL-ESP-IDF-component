# Image From SD Card

Display a JPEG image from an SD card on Soldered Inkplate 6Flick using LVGL.

## Overview

Initialises the SD card, loads `cat.jpg` from the root directory using the LVGL SD filesystem driver (registered automatically by the Inkplate constructor), displays it, then puts the SD card to sleep for power saving.

## Hardware Required

- Soldered Inkplate 6Flick
- USB cable
- microSD card with `cat.jpg` in the root directory (FAT formatted)

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate6 Flick**

Copy `cat.jpg` to the root of a FAT-formatted microSD card and insert it into the Inkplate 6Flick.

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

The cat image loaded from SD card and shown on the display.

## Notes

- SD filesystem path prefix is `S:/` (e.g. `"S:/cat.jpg"`).
- `display.enableDithering(true)` improves image quality on the e-paper panel.
- `display.sdCardSleep()` powers down the SD card after use.
- `display.frontlight.setState(false)` disables the frontlight at startup.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
