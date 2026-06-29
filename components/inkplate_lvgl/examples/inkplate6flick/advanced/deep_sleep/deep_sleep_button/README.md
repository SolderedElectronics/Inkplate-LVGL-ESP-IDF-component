# Deep Sleep Touch Wake

Deep sleep with touchscreen wakeup example for Soldered Inkplate 6Flick.

## Overview

Wakes from deep sleep either on a 30-second timer or when the touchscreen interrupt line (GPIO 36) goes low (any touch event). The wake reason and boot count are shown on the display before returning to deep sleep.

## Hardware Required

- Soldered Inkplate 6Flick
- USB cable

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate6 Flick**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

- Display shows boot count and wake reason ("Wakeup: Touch" or "Wakeup: Timer").
- Device enters deep sleep after each display update.
- Touch the screen or wait 30 s to wake again.

## Notes

- GPIO 36 is the touchscreen INT pin (active-low, EXT0 wakeup). There is no physical WAKE button on Inkplate 6Flick — touch is the button-equivalent wake source.
- `RTC_DATA_ATTR` preserves `bootCount` across deep sleep cycles.
- `display.frontlight.setState(false)` disables the frontlight at startup.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
