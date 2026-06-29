# Frontlight

Frontlight brightness control example for Inkplate 4.

## Overview

Sweeps the built-in LED frontlight from minimum (0) to maximum (63) brightness and back in a continuous loop. The current brightness level is shown on the e-paper display and updated every 8 steps.

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

- Frontlight LEDs brighten and dim continuously.
- Display updates every 8 brightness steps showing the current level (0–63).

## Notes

- `display.frontlight.setState(true)` enables the frontlight.
- `display.frontlight.setBrightness(n)` sets brightness from 0 (off) to 63 (max).
- `display.partialUpdate()` is used for fast display updates.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
