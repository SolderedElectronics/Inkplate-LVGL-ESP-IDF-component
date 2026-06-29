# External IO Expander

Blink an LED via the external IO expander on Soldered Inkplate 6 using LVGL.

## Overview

Blinks an LED connected to the external IO expander pin P1-7 (GPB7 = IO_NUM_B7). All pins on the external expander are free to use — nothing is internally connected.

## Hardware Required

- Soldered Inkplate 6
- 330 Ohm resistor
- LED: anode → resistor → IO Expander 2 pin P1-7 (GPB7), cathode → GND

## Setup

1. Wire LED + 330 Ohm resistor to P1-7 on the IO Expander 2 header.

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate6**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

LED blinks once per second. Display shows "External IO expander — Blinking LED on P1-7 (GPB7)".

## Notes

- All pins on the external expander are safe to use.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
