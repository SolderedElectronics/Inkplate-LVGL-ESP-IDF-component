# RTC Simple Clock

RTC simple clock example for Soldered Inkplate 4.

## Overview

Sets the RTC date/time, displays the current time, and refreshes the display every 60 seconds.

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

Display shows current time and date, updating every 60 s.

## Notes

- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
