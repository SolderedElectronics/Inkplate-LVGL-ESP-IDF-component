# RTC Alarm

RTC alarm example for Soldered Inkplate 13.

## Overview

Sets RTC time and date, then sets an alarm 1 minute ahead. Displays current time every 60 s. When the alarm fires, "ALARM!" appears in green on screen.

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

- Display shows current time.
- "ALARM!" appears in green after approximately 1 minute.

## Notes

- `display.enableDithering(true)` is required for correct color rendering.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
