# RTC Alarm Deep Sleep

RTC alarm deep sleep example for Soldered Inkplate 6Flick.

## Overview

Sets an RTC alarm 60 seconds ahead, displays the current time, then enters deep sleep. The RTC alarm interrupt (GPIO 39) wakes the device and the display is refreshed with the new time.

## Hardware Required

- Soldered Inkplate 6Flick
- USB cable
- CR2032 battery (optional, keeps RTC running without USB power)

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate6 Flick**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

- Display shows current time and enters deep sleep.
- Device wakes every 60 s via RTC alarm interrupt.

## Notes

- GPIO 39 is the RTC INT pin (active-low alarm output).
- RTC time is set once on first boot and persists across deep sleep.
- `display.frontlight.setState(false)` disables the frontlight at startup.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
