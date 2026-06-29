# RTC Alarm Interrupt

RTC alarm interrupt example for Soldered Inkplate 6Flick.

## Overview

Sets RTC time to 14:30:00 and an alarm at 14:31:00. The RTC INT pin (GPIO 39) triggers a GPIO interrupt which notifies the main task via FreeRTOS task notification. When the alarm fires the display shows "ALARM!". The display also refreshes every 60 s via timeout if no interrupt arrives sooner.

## Hardware Required

- Soldered Inkplate 6Flick
- USB cable
- CR2032 battery (optional, keeps RTC running without USB power)

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate6 Flick**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

- Display shows current time.
- "ALARM!" appears approximately 60 s after boot.

## Notes

- GPIO 39 is the RTC INT pin (active-low, open-drain with external pull-up).
- The touchscreen driver installs the GPIO ISR service in the Inkplate constructor — this example uses only `gpio_isr_handler_add` without re-installing the service.
- `GPIO_INTR_LOW_LEVEL` used — reliable even if the falling edge occurs during display refresh.
- ISR disables its own interrupt; main loop re-enables after `clearAlarmFlag()`.
- `display.frontlight.setState(false)` disables the frontlight at startup.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
