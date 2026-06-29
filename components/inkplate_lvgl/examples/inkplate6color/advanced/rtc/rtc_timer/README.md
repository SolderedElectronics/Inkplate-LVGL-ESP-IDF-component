# RTC Timer

Display a live clock with RTC countdown timer on Soldered Inkplate 6Color using LVGL.

## Overview

Configures the RTC countdown timer for 60 seconds at 1 Hz. When the timer fires, "Timer!" is appended to the displayed time string. The display refreshes every 60 seconds.

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

Time updates every 60 s; "Timer!" appears when the countdown finishes.

## Notes

- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
