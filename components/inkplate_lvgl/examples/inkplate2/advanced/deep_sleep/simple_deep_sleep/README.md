# Simple Deep Sleep

Demonstrate ESP32 timer-based deep sleep on Soldered Inkplate 2 using LVGL.

## Overview

Demonstrates ESP32 deep sleep with timer wakeup. On each wake, the wake count is incremented (persisted via `RTC_DATA_ATTR`), displayed on the e-paper screen, then the device returns to sleep.

## Hardware Required

- Soldered Inkplate 2
- USB cable

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate2**

Adjust `TIME_TO_SLEEP` in `main.cpp` to change the sleep duration (default: 30 seconds).

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

Display shows "Wake count: N" updating every `TIME_TO_SLEEP` seconds.

## Notes

- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.
- Wake count is stored in RTC memory and survives deep sleep resets.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
