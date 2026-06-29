# APDS9960 Sensor

APDS9960 proximity, gesture, and light sensor example for Inkplate 4.

## Overview

Reads proximity, gesture direction, ambient light, and RGB color from the built-in APDS9960 sensor. Display updates via partial refresh; full refresh every 30 updates.

## Hardware Required

- Soldered Inkplate 4
- USB cable

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate4**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

- Display shows proximity, last gesture, ambient light, and RGB values.
- Wave a hand over the sensor to trigger gestures; bring it close for proximity readings.

## Notes

- `display.apds` provides access to the APDS9960 driver.
- `display.partialUpdate()` is used for fast live updates; full refresh every 30 partials.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
