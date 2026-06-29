# Burn-In Clean

Burn-in cleaning example for Soldered Inkplate 6Flick.

## Overview

Runs multiple full-panel cleaning cycles to reduce burn-in visible on the e-paper panel. Set `CLEAR_CYCLES` and `CYCLES_DELAY` as needed, then flash and wait for the "Cleaning Done!" message.

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

- Display shows "Running burn-in clean... Please wait." during cycles.
- "Cleaning Done!" appears when all cycles complete.

## Notes

- `CLEAR_CYCLES` sets the number of cleaning iterations (default: 20).
- `CYCLES_DELAY` sets the delay in milliseconds between each cycle (default: 5000).
- `display.frontlight.setState(false)` disables the frontlight at startup.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
