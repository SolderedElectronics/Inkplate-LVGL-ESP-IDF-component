# RTC Alarm Interrupt

Trigger a GPIO interrupt from an RTC alarm on Soldered Inkplate 6Color using LVGL.

## Overview

Configures an RTC alarm 60 seconds from a fixed epoch start time. When the alarm fires, the RTC INT pin (GPIO39) triggers a GPIO interrupt which sets a flag. On the next screen refresh the flag is checked and "ALARM!" is shown in red.

## Hardware Required

- Soldered Inkplate 6Color
- USB cable

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate6 Color**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

Time updates every 60 s; "ALARM!" appears in red when interrupt fires.

## Notes

- GPIO 39 is the RTC INT pin (active-low alarm output). GPIO 39 is input-only on ESP32 — no internal pull-up available.
- `GPIO_INTR_LOW_LEVEL` used instead of edge — reliable even if the falling edge occurs while display is refreshing.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
