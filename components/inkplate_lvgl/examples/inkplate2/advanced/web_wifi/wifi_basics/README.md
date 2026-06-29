# WiFi Basics

Connect to a WiFi network and display connection status on Soldered Inkplate 2 using LVGL.

## Overview

Demonstrates how to connect the Inkplate 2 to a WiFi network and display the connection status on the e-paper screen. WiFi credentials are set via menuconfig.

## Hardware Required

- Soldered Inkplate 2
- USB cable
- Stable WiFi network

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate2**
**WiFi Configuration → Enter your SSID and password**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

- Display shows "Connected!" on success or "WiFi failed!" on failure.
- Serial monitor shows connection progress.

## Notes

- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
