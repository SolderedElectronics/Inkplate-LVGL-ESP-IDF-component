# External IO Expander

External IO expander example for Soldered Inkplate 13.

## Overview

Demonstrates using the onboard PCAL6416A IO expander by blinking an LED connected to pin P1-7 (IO_NUM_B7, GPB7).

## Hardware Required

- Soldered Inkplate 13
- USB cable
- 330 Ω resistor
- LED

Wiring:
- LED anode → 330 Ω resistor → IO Expander pin P1-7 (GPB7)
- LED cathode → GND

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate13**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

LED blinks once per second.

## Notes

- `expander1` is declared `extern PCAL expander1` (defined in BoardCommon.cpp).
- Pin mapping: P0-0 = IO_NUM_A0, ..., P1-7 = IO_NUM_B7.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
