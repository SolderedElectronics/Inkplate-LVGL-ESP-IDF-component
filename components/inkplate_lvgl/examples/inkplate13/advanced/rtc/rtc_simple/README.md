# RTC Simple

RTC simple clock example for Soldered Inkplate 13.

## Overview

Sets the RTC date and time, displays the current time, and refreshes the display every 60 seconds.

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

Display shows current time and date (HH:MM:SS and weekday/date), updating every 60 s.

## Notes

- `display.enableDithering(true)` is required for correct color rendering.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
