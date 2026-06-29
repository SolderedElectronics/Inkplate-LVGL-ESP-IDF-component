# Read Battery Voltage

Read and display LiPo battery voltage on Soldered Inkplate 5 using LVGL.

## Overview

Reads the LiPo battery voltage via the ESP32 ADC and displays it on the e-ink panel. The display refreshes every 10 seconds using fast partial update.

## Hardware Required

- Soldered Inkplate 5
- USB cable
- LiPo battery (optional)

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate5**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

Display shows battery voltage in volts, updating every 10 s.

## Notes

- Without a battery connected the reading shows the USB supply (~4.2 V).
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.partialUpdate()` is used for fast refresh between full updates.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
