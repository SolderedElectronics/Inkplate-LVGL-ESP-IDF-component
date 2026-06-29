# Set VCOM

VCOM voltage setup example for Soldered Inkplate 6Flick.

## Overview

Displays 8 grayscale shades for visual quality assessment, then accepts a VCOM voltage value entered via the serial terminal. The value is written to the TPS65186 PMIC EEPROM and ESP32 NVS.

## Hardware Required

- Soldered Inkplate 6Flick
- USB cable

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate6 Flick**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

- Display shows 8 grayscale bars and current VCOM value.
- Open serial terminal at 115200 baud.
- Enter VCOM voltage (e.g. `-1.23`) and press Enter.
- Display refreshes after VCOM is written.

## Notes

> **WARNING:** VCOM is stored in the TPS65186 PMIC EEPROM which supports a **limited number of write cycles**. Do NOT run this example repeatedly. Set VCOM once.

- VCOM must be in the range `[-5.0, 0.0]` volts.
- The optimal VCOM value is sometimes printed on the flat cable connector.
- `display.frontlight.setState(false)` disables the frontlight at startup.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
