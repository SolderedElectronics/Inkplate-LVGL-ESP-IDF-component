# Touchscreen

Capacitive touchscreen example for Inkplate 4.

## Overview

Polls the built-in Elan capacitive touchscreen. When a touch is detected the coordinates are shown and a filled rectangle moves to the touch position. Partial refresh is used for fast updates.

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

Touch the screen — a black square follows your finger.

## Notes

- Touch events are detected via a FreeRTOS semaphore given from the GPIO ISR.
- `display.touchscreen.getData(x, y)` returns the number of touch points and their coordinates.
- `display.partialUpdate()` is used for fast live updates; full refresh every 30 partials.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
