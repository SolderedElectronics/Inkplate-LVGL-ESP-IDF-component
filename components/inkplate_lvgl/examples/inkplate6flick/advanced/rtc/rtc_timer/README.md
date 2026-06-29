# RTC Timer

RTC countdown timer with live clock example for Soldered Inkplate 6Flick.

## Overview

Uses the PCF85063A countdown timer to fire once after 60 seconds. The display updates every second using fast partial refresh. A full refresh is performed every 60 partials to reduce ghosting. "Timer!" is appended to the label when the countdown completes.

## Hardware Required

- Soldered Inkplate 6Flick
- USB cable
- CR2032 battery (optional, keeps RTC running without USB power)

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate6 Flick**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

- Display shows a live clock updating every second.
- "Timer!" appears after 60 s.
- A full refresh runs every 60 partial updates to prevent ghosting.

## Notes

- `display.frontlight.setState(false)` disables the frontlight at startup.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.partialUpdate()` refreshes only the changed region for faster updates.
- `display.display()` is called every 60 s for a full panel refresh.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
