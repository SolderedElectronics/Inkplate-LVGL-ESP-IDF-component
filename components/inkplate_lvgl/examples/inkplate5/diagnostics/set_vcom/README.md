# Set VCOM

Set the VCOM voltage for optimal image quality on Soldered Inkplate 5 using LVGL.

## Overview

**WARNING:** VCOM voltage is stored in the TPS65186 PMIC EEPROM and in ESP32 NVS. The PMIC EEPROM supports a limited number of write cycles, so do NOT run this example repeatedly. Set VCOM once.

Displays 8 grayscale shades for visual quality assessment. Enter the VCOM value via serial to program it.

## Hardware Required

- Soldered Inkplate 5
- USB cable

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate5**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

- Display shows 8 grayscale shades and current VCOM value.
- Open a serial terminal at 115200 baud.
- Enter VCOM voltage (e.g. `-1.23`) and press Enter.
- VCOM is written to PMIC EEPROM and NVS; display refreshes.

## Notes

- VCOM must be in the range [–5.0, 0.0] volts.
- The optimal VCOM is sometimes printed on the flat cable connector.
- Do NOT run this example repeatedly — PMIC EEPROM has limited write cycles.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
