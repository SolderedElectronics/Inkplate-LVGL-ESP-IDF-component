# RTC Timer

RTC timer + live clock example for Soldered Inkplate 4.

## Overview

Sets an RTC countdown timer for 60 seconds. Displays current time updating every second via partial refresh. When the timer fires, "Timer!" is shown on the display.

## Hardware Required

- Soldered Inkplate 4
- USB cable
- CR2032 battery (optional, keeps RTC running when USB is disconnected)

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate4**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

Display shows a live clock; "Timer!" appears after 60 s.

## Notes

- `display.partialUpdate()` is used for fast second-by-second refresh; a full refresh runs every 60 partials to prevent ghosting.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
