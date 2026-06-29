# Deep Sleep Button Wake

Deep sleep with button wake example for Soldered Inkplate 10.

## Overview

Wakes from deep sleep either on a 30-second timer or when the WAKE button (GPIO 36) is pressed (active-low). The wake reason and boot count are shown on the display before returning to deep sleep.

## Hardware Required

- Soldered Inkplate 10
- USB cable

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate10**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

- Display shows boot count and wake reason ("Wakeup: Button" or "Wakeup: Timer").
- Device enters deep sleep after each display update.
- Press WAKE button or wait 30 s to wake again.

## Notes

- GPIO 36 is the WAKE button pin (active-low, EXT0 wakeup).
- `RTC_DATA_ATTR` preserves `bootCount` across deep sleep cycles.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
