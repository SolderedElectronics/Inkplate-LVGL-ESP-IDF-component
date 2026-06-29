# BME680 Environmental Sensor

BME680 environmental sensor example for Inkplate 4.

## Overview

Reads temperature, humidity, pressure, gas resistance, and calculated altitude from the built-in BME680 sensor. Display updates via partial refresh every second; full refresh every 10 updates.

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

Display shows live environmental readings updating every second.

## Notes

- `display.bme` provides access to the BME680 driver.
- `display.partialUpdate()` is used for fast live updates; full refresh every 10 partials.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
