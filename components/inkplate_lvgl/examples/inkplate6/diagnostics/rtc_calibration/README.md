# RTC Calibration

Calibrate the PCF85063A RTC crystal frequency on Soldered Inkplate 6 using LVGL.

## Overview

Initialises the PCF85063A RTC with default calibration settings and prints HH:MM:SS to the serial console every second. Adjust `setClockOffset()` to fine-tune the crystal frequency.

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

- Display shows "RTC calibration — Open Serial Monitor 115200 baud."
- Serial monitor prints `HH:MM:SS` every second starting from 00:00:00.
- Compare with a reference clock; adjust `setClockOffset()` as needed.

## Notes

- `setInternalCapacitor(true)` selects the 12.5 pF internal load capacitor.
- `setClockOffset(mode, value)`: mode 1 = correction every 4 minutes; value is two's complement offset (–63 to +63).

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
