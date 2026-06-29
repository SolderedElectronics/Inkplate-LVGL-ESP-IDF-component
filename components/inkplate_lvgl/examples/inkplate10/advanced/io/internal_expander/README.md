# Internal IO Expander

Internal IO expander example for Soldered Inkplate 10.

## Overview

Blinks an LED connected to the internal IO expander pin P1-7 (GPB7). Only Port B pins GPB1–GPB7 (IO_NUM_B1–B7) are safe to use — Port A and GPB0 are reserved for the display and PMIC.

## Hardware Required

- Soldered Inkplate 10
- USB cable
- 330 Ω resistor
- LED

Wiring:
- LED anode → 330 Ω resistor → IO Expander 1 pin P1-7 (GPB7)
- LED cathode → GND

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate10**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

- Display shows "Internal IO expander — Blinking LED on P1-7 (GPB7)".
- LED blinks once per second.

## Notes

> **WARNING:** Do NOT use pins P0-0 to P0-7 (GPA0–GPA7) or P1-0 (GPB0). These are reserved for the e-paper panel and PMIC. Using them may permanently damage the display.

- Safe pins on internal expander: GPB1–GPB7 (IO_NUM_B1–B7) only.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
