# Internal IO Expander

Blink an LED via the internal IO expander on Soldered Inkplate 6 using LVGL.

## Overview

Blinks an LED connected to the internal IO expander pin P1-7 (GPB7 = IO_NUM_B7). Only Port B pins (GPB1–GPB7, IO_NUM_B1–B7) are safe to use; Port A and GPB0 are reserved for the display.

## Hardware Required

- Soldered Inkplate 6
- 330 Ohm resistor
- LED: anode → resistor → IO Expander 1 pin P1-7 (GPB7), cathode → GND

## Setup

1. Wire LED + 330 Ohm resistor to P1-7 on the IO Expander 1 header.

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate6**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

LED blinks once per second. Display shows "Internal IO expander — Blinking LED on P1-7 (GPB7)".

## Notes

**WARNING:** DO NOT use P0-0 to P0-7 (GPA0–GPA7) or P1-0 (GPB0) — reserved for the e-paper panel and PMIC. Using them may permanently damage the screen.

- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
