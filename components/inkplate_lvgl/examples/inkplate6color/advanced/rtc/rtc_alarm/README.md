# RTC Alarm

Set an RTC alarm and display "ALARM!" on Soldered Inkplate 6Color using LVGL.

## Overview

Sets the RTC time, date, and an alarm 1 minute in the future. The display refreshes every 60 seconds. When the alarm fires, "ALARM!" is shown in green on screen.

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

Time updates every 60 s; "ALARM!" appears when the alarm triggers.

## Notes

- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
