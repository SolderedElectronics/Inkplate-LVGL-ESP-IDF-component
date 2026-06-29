# Partial Update

Partial update moving square example for Soldered Inkplate 10 using LVGL.

## Overview

Demonstrates partial update mode by moving a square across the screen. Only the changed regions are refreshed, making updates faster than a full display refresh.

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

A square moves diagonally across the 1200×825 screen using partial updates.

## Notes

- `LV_DISPLAY_RENDER_MODE_PARTIAL` must be used for `display.partialUpdate()` to work.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.partialUpdate()` pushes only the changed region to the e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
