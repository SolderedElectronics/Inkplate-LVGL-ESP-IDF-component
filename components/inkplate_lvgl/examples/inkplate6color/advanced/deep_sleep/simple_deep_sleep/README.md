# Simple Deep Sleep

Demonstrate ESP32 timer-based deep sleep on Soldered Inkplate 6Color using LVGL.

## Overview

Puts the device into deep sleep, wakes it up every 30 seconds via the RTC timer, and displays the wakeup count on the e-paper screen. The counter is preserved across deep sleep cycles using RTC memory.

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

Device displays wake count, then sleeps for 30 seconds. On each wakeup the count increments and the screen refreshes.

## Notes

- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.
- Wake count is stored in RTC memory and survives deep sleep resets.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
