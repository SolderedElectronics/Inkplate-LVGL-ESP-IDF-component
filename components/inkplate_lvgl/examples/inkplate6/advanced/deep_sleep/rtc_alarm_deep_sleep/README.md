# RTC Alarm Deep Sleep

Wake Soldered Inkplate 6 from deep sleep via RTC alarm every 60 seconds using LVGL.

## Overview

Sets an RTC alarm 60 seconds ahead, displays current time, then enters deep sleep. The RTC alarm interrupt (GPIO 39) wakes the device and the display is refreshed with the new time.

## Hardware Required

- Soldered Inkplate 6
- USB cable
- CR2032 battery (optional, keeps RTC running without USB power)

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate6**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

Display shows current time and enters deep sleep. Device wakes every 60 s via RTC alarm.

## Notes

- The RTC time is set once (if not already set) and persists across deep sleep.
- GPIO 39 is the RTC INT pin (active-low alarm output).
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
