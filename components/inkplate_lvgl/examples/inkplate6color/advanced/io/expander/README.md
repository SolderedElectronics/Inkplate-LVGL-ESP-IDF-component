# IO Expander

Toggle all IO expander pins on Soldered Inkplate 6Color using LVGL.

## Overview

Demonstrates using the onboard MCP23017 I/O expander by toggling all 16 pins (P0-0 to P1-7) between LOW and HIGH every second. Connect an LED with a 330 Ohm resistor to any expander pin to see it blink.

## Hardware Required

- Soldered Inkplate 6Color
- USB cable
- LED + 330 Ohm resistor (optional)

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate6 Color**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

All expander pins toggle every 1 second. LED blinks if connected.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
