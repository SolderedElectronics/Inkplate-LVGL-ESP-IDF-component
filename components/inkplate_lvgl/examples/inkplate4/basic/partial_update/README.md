# Partial Update

Partial update moving square example for Soldered Inkplate 4 using LVGL.

## Overview

Demonstrates partial update mode by moving a square across the screen. Only the changed regions are refreshed, making updates faster than a full display refresh.

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

A square moves diagonally across the screen using partial updates.

## Notes

- `LV_DISPLAY_RENDER_MODE_PARTIAL` enables partial refresh; only changed regions are sent to the panel.
- `display.partialUpdate()` pushes the dirty region to the e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
