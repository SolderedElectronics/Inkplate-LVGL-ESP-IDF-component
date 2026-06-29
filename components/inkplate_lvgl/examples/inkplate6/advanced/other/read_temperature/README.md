# Read Temperature

Read and display panel temperature on Soldered Inkplate 6 using LVGL.

## Overview

Reads the panel temperature from the TPS65186 PMIC thermistor and displays it on the e-ink panel. The display refreshes every 10 seconds using fast partial update.

## Hardware Required

- Soldered Inkplate 6
- USB cable

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate6**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

Display shows panel temperature in Celsius, updating every 10 s.

## Notes

- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.partialUpdate()` is used for fast refresh between full updates.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
