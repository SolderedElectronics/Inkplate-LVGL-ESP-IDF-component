# RTC Alarm

RTC alarm example for Soldered Inkplate 4.

## Overview

Sets RTC time/date and an alarm 1 minute ahead. Displays current time every 60 s. When the alarm fires, "ALARM!" appears on screen.

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

Display shows current time; "ALARM!" appears after ~1 minute.

## Notes

- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
