# Simple Deep Sleep

Simple deep sleep example for Soldered Inkplate 4.

## Overview

Puts Inkplate into deep sleep and wakes every 30 seconds. A counter retained in RTC memory tracks the number of wakes and is displayed on the e-ink panel on each boot.

## Hardware Required

- Soldered Inkplate 4
- USB cable

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate4**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

Display shows wake count and enters deep sleep every 30 s.

## Notes

- `RTC_DATA_ATTR` preserves the wake counter across deep sleep cycles.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
