# RTC Calibration

Calibrate the PCF85063A RTC crystal frequency on Soldered Inkplate 7 using LVGL.

## Overview

Sets the internal load capacitor and clock offset, waits for the WAKEUP button press, then starts the RTC at 00:00:00 and prints HH:MM:SS to the serial console every second. Adjust `setClockOffset()` to fine-tune the crystal frequency.

If the RTC drifts, adjust the offset value — negative values slow the clock, positive speed it up. Compare against a reference clock over several minutes.

## Hardware Required

- Soldered Inkplate 7
- USB cable

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate7**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

- Display prompts to open serial monitor, then to press the WAKEUP button.
- After button press (GPIO 18), RTC starts at 00:00:00 and prints `HH:MM:SS` every second.
- Open serial monitor at 115200 baud.
- Compare with a reference clock; adjust `setClockOffset()` as needed.

## Notes

- `setInternalCapacitor(true)` selects the 12.5 pF internal load capacitor.
- `setClockOffset(mode, value)`: mode 1 = correction every 4 minutes; value is two's complement offset (–63 to +63).
- GPIO 18 is the WAKEUP button pin (active-low, internal pull-up enabled).
- `display.enableDithering(true)` is required for correct color rendering.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
