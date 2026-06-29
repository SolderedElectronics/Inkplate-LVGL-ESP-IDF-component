# LSM6DS3 IMU

LSM6DS3 accelerometer and gyroscope example for Inkplate 4.

## Overview

Reads X/Y/Z acceleration (in g) and X/Y/Z angular rate (deg/s) from the built-in LSM6DS3 IMU. Display updates via partial refresh; full refresh every 10 updates.

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

- Display shows live accelerometer and gyroscope values.
- Tilt the board to change accelerometer readings.

## Notes

- `display.lsm` provides access to the LSM6DS3 driver.
- `display.partialUpdate()` is used for fast live updates; full refresh every 10 partials.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
