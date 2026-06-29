# RTC Calibration

Calibrate the PCF85063A RTC crystal frequency on Soldered Inkplate 6Color using LVGL.

## Overview

Demonstrates how to calibrate the PCF85063A RTC for improved time accuracy. Sets the internal capacitor and clock offset, then tracks time after a wake button press. Time is printed to serial every second; display shows status prompts.

If the RTC drifts, adjust the offset value passed to `setClockOffset()` — negative values slow the clock, positive speed it up. Compare against a reference clock over several minutes.

## Hardware Required

- Soldered Inkplate 6Color
- USB cable

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate6 Color**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

- Display shows status prompts.
- Open serial monitor at 115200 baud.
- Press the wake button when prompted.
- Serial monitor prints `HH:MM:SS` every second after button press.

## Notes

- `setInternalCapacitor(true)` selects the 12.5 pF internal load capacitor.
- `setClockOffset(mode, value)`: mode 1 = correction every 4 minutes; value is two's complement offset (–63 to +63).

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
