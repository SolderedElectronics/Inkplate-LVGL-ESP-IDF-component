# Simple Touchscreen

Simple touchscreen example for Soldered Inkplate 6Flick using LVGL.

## Overview

Demonstrates direct touchscreen polling on the Inkplate 6Flick. A rectangle is drawn on screen; each tap moves it diagonally until it reaches the bottom, then it resets to the top-left.

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

- "Touch the rectangle on screen!" prompt shown for 2 s.
- Black rectangle appears at the top-left.
- Tap the rectangle to move it diagonally by 100 px each touch.
- Resets to top-left when it reaches the bottom of the screen.

## Notes

- Touchscreen is initialized automatically by the Inkplate constructor.
- Touch events are detected via a FreeRTOS semaphore given from the GPIO ISR.
- `display.frontlight.setState(false)` disables the frontlight at startup.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.partialUpdate()` refreshes only the changed region.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
