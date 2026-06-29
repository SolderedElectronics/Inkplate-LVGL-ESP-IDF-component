# EEPROM Usage (NVS)

NVS (EEPROM equivalent) usage example for Soldered Inkplate 10.

## Overview

Demonstrates storing and reading persistent data using NVS (Non-Volatile Storage), which replaces Arduino EEPROM in ESP-IDF. Writes 128 bytes to the "user_data" NVS namespace, reads them back, and verifies the data. Result is shown on the display.

## Hardware Required

- Soldered Inkplate 10
- USB cable

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate10**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

Display shows "NVS OK! All 128 bytes verified." on success, or an error message on failure.

## Notes

- NVS "user_data" namespace is separate from Inkplate's internal "inkplate" namespace — no conflict.
- Data persists across reboots until explicitly erased.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
