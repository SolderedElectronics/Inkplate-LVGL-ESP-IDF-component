# WiFi Basics

Connect to a WiFi network and display connection status on Soldered Inkplate 7 using LVGL.

## Overview

Connects to a WiFi network using credentials set via menuconfig and displays the connection result on the e-paper screen.

## Hardware Required

- Soldered Inkplate 7
- USB cable
- Stable WiFi connection

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate7**
**WiFi Configuration → Enter your SSID and password**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

"Connected!" appears on screen when WiFi is established.

## Notes

- `display.enableDithering(true)` is required for correct color rendering.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
