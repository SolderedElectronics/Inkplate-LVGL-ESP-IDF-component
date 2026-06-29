# External IO Expander

External IO expander example for Soldered Inkplate 6Flick.

## Overview

Blinks an LED connected to the external IO expander (expander2) pin P1-7 (GPB7). All pins on the external expander are free to use — nothing is internally connected.

## Hardware Required

- Soldered Inkplate 6Flick
- USB cable
- 330 Ω resistor
- LED

Wiring:
- LED anode → 330 Ω resistor → IO Expander 2 pin P1-7 (GPB7)
- LED cathode → GND

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate6 Flick**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

- Display shows "External IO expander — Blinking LED on P1-7 (GPB7)".
- LED blinks once per second.

## Notes

- `expander2` is declared `extern PCAL expander2` (defined in BoardCommon.cpp).
- All pins on the external expander are free to use.
- `display.frontlight.setState(false)` disables the frontlight at startup.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
