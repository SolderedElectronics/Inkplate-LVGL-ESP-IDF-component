# RTC Alarm Interrupt

RTC alarm interrupt example for Soldered Inkplate 7.

## Overview

Sets RTC time to 14:30:00 and an alarm at 14:31:00. The RTC INT pin (GPIO 18) triggers a GPIO interrupt which notifies the main task via FreeRTOS task notification. When the alarm fires the display shows "ALARM!" in red. The display also refreshes every 60 s via timeout if no interrupt arrives sooner.

## Hardware Required

- Soldered Inkplate 7
- USB cable
- CR2032 battery (optional, keeps RTC running without USB power)

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate7**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

- Display shows current time.
- "ALARM!" appears in red approximately 60 s after boot.

## Notes

- GPIO 18 is shared between the RTC INT pin and the WAKEUP button on Inkplate 7.
- `GPIO_INTR_LOW_LEVEL` used — reliable even if the falling edge occurs during display refresh.
- ISR disables its own interrupt; main loop re-enables after `clearAlarmFlag()`.
- `display.enableDithering(true)` is required for correct color rendering.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
