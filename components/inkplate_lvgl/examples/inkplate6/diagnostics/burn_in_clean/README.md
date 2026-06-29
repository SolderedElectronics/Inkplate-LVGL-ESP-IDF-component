# Burn-In Clean

Run a burn-in cleaning cycle to reduce ghosting on the Inkplate 6 e-paper panel.

## Overview

Runs multiple full-panel cleaning cycles to reduce burn-in visible on the e-ink panel. Set `CLEAR_CYCLES` and `CYCLES_DELAY` as needed, then flash and wait for the "Cleaning Done!" message.

## Hardware Required

- Soldered Inkplate 6
- USB cable

## Setup

In `main.cpp`, configure:
- `CLEAR_CYCLES` — number of cleaning cycles (default: 20)
- `CYCLES_DELAY` — delay in milliseconds between cycles (default: 5000, keep ≥ 5000)

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate6**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

Display cycles through cleaning, then shows "Cleaning Done!".

## Notes

- Do not set `CYCLES_DELAY` below 5000 ms between cycles.
- Do not interrupt power during the cleaning sequence.
- Increase `CLEAR_CYCLES` for more severe ghosting.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
