# RTC Alarm Interrupt

Trigger a GPIO interrupt from an RTC alarm on Soldered Inkplate 5 using LVGL.

## Overview

Sets an RTC alarm 60 seconds after a known epoch. The RTC INT pin (GPIO 39) triggers a GPIO interrupt which sets a flag. When the alarm fires the display shows "ALARM!".

## Hardware Required

- Soldered Inkplate 5
- USB cable
- CR2032 battery (optional, keeps RTC running without USB power)

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate5**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

Display shows current time; "ALARM!" appears ~60 s after boot.

## Notes

- GPIO 39 is the RTC INT pin (active-low alarm output).
- `GPIO_INTR_LOW_LEVEL` used instead of edge — reliable even if the falling edge occurs while display is refreshing (interrupts temporarily off).
- ISR disables its own interrupt; main loop re-enables after `clearAlarmFlag()`.
- Task notification used for ISR-to-task signalling (safer than volatile flag on dual-core ESP32).
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
