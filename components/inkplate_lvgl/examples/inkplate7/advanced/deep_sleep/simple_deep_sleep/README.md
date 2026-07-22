# Simple Deep Sleep

Simple deep sleep timer wakeup example for Soldered Inkplate 7.

## Overview

Puts the device into deep sleep and wakes it every 30 seconds via timer. A counter retained in RTC memory tracks the number of wakes and is displayed on the e-paper screen on each boot.

## Hardware Required

- Soldered Inkplate 7
- USB cable

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate7**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

Display shows wake count, then device sleeps for 30 s. Count increments on each wakeup.

## Notes

- `RTC_DATA_ATTR` preserves `wakeCount` across deep sleep cycles.
- `display.enableDithering(true)` is required for correct color rendering.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
