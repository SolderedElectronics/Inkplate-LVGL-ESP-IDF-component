# RTC Timer

RTC countdown timer with live clock example for Soldered Inkplate 10.

## Overview

Sets an RTC countdown timer for 60 seconds. Displays the current time updating every second via partial refresh. When the timer fires, "Timer!" is appended to the display.

## Hardware Required

- Soldered Inkplate 10
- USB cable
- CR2032 battery (optional, keeps RTC running without USB power)

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate10**

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

- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.partialUpdate()` refreshes only the changed region for faster updates.
- `display.display()` is called every 60 s for a full panel refresh.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
