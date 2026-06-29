# RTC Timer

RTC countdown timer example for Soldered Inkplate 13.

## Overview

Configures the RTC countdown timer for 60 seconds at 1 Hz. Displays current time updating every 60 seconds. When the timer fires, "Timer!" is appended to the displayed time string.

## Hardware Required

- Soldered Inkplate 13
- USB cable
- CR2032 battery (optional, keeps RTC running without USB power)

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate13**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

- Display shows current time updating every 60 s.
- "Timer!" appears when the 60-second countdown finishes.

## Notes

- `display.enableDithering(true)` is required for correct color rendering.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
