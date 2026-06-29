# Read Battery Voltage

Read and display LiPo battery voltage on Soldered Inkplate 6Color using LVGL.

## Overview

Reads the LiPo battery voltage via the onboard ADC and displays it on screen. The display refreshes every 60 seconds.

## Hardware Required

- Soldered Inkplate 6Color
- USB cable
- LiPo battery (optional)

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate6 Color**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

Battery voltage updates on screen every 60 seconds.

## Notes

- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
