# Read Battery Voltage

Battery voltage reading example for Soldered Inkplate 6Flick.

## Overview

Reads the LiPo battery voltage via the ESP32 ADC and displays it on the e-paper panel. The display refreshes every 10 seconds using fast partial update.

## Hardware Required

- Soldered Inkplate 6Flick
- USB cable
- LiPo battery (optional)

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate6 Flick**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

Display shows battery voltage in volts (e.g. "3.85 V"), updating every 10 s.

## Notes

- Without a battery connected the reading reflects the USB supply (~4.2 V).
- `display.frontlight.setState(false)` disables the frontlight at startup.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.partialUpdate()` refreshes only the changed region for faster updates.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
