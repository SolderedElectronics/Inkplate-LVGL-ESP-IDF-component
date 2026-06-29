# Read Temperature

Temperature reading example for Soldered Inkplate 10.

## Overview

Reads the panel temperature from the TPS65186 PMIC thermistor and displays it on the e-paper panel. The display refreshes every 10 seconds using fast partial update.

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

Display shows panel temperature in Celsius (e.g. "25 C"), updating every 10 s.

## Notes

- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.partialUpdate()` refreshes only the changed region for faster updates.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
