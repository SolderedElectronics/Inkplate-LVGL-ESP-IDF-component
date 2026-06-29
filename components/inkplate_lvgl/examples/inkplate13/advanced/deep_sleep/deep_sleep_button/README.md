# Deep Sleep Button Wake

Deep sleep with button wakeup example for Soldered Inkplate 13.

## Overview

Demonstrates waking the ESP32-S3 from deep sleep via two sources: the WAKEUP button (GPIO 18, EXT0 wakeup) or a 30-second timer. The boot counter and wakeup reason are displayed on screen before returning to deep sleep.

## Hardware Required

- Soldered Inkplate 13
- USB cable

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate13**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

- Display shows boot count and wake reason ("Wakeup: Button", "Wakeup: Timer", or "Wakeup: Power on / Reset").
- Device enters deep sleep after each display update.
- Press WAKEUP button (GPIO 18) or wait 30 s to wake again.

## Notes

- GPIO 18 is the WAKEUP button pin (active-low, EXT0 wakeup).
- `RTC_DATA_ATTR` preserves `bootCount` across deep sleep cycles.
- `display.enableDithering(true)` is required for correct color rendering.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
