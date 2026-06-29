# Burn-In Clean

Run a burn-in cleaning cycle to reduce ghosting on the Inkplate 2 e-paper panel.

## Overview

Attempts to reduce e-paper burn-in by repeatedly refreshing the display with a blank white frame. After all cycles complete, a "Clearing Done!" message is shown on screen.

## Hardware Required

- Soldered Inkplate 2
- USB cable

## Setup

In `main.cpp`, configure:
- `CLEAR_CYCLES` — number of cleaning cycles (default: 5)
- `CYCLES_DELAY_MS` — delay between cycles in milliseconds (default: 5000, keep ≥ 5000)

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate2**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

Display flashes white `CLEAR_CYCLES` times, then shows "Clearing Done!".

## Notes

- Do not set `CYCLES_DELAY_MS` below 5000 ms between cycles.
- Do not interrupt power during the cleaning sequence.
- Increase `CLEAR_CYCLES` for more severe ghosting.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
