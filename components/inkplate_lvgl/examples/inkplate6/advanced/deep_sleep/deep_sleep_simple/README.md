# Simple Deep Sleep

Demonstrate ESP32 timer-based deep sleep on Soldered Inkplate 6 using LVGL.

## Overview

Puts Inkplate into deep sleep and wakes every 30 seconds. A counter retained in RTC memory tracks the number of wakes and is displayed on the e-ink panel on each boot.

## Hardware Required

- Soldered Inkplate 6
- USB cable

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate6**

Adjust `TIME_TO_SLEEP` in `main.cpp` to change the sleep duration (default: 30 seconds).

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

Display shows wake count and enters deep sleep every 30 s.

## Notes

- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.
- Wake count is stored in RTC memory and survives deep sleep resets.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
