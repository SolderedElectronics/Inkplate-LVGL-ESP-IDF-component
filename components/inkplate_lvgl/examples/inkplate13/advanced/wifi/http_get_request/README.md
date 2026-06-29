# HTTP GET Request

Fetch a webpage via HTTP GET and display the response on Soldered Inkplate 13 using LVGL.

## Overview

Connects to WiFi, performs an HTTP GET to example.com, and displays the raw HTML response body on the e-paper screen. No HTML parsing is performed — the raw text is shown as-is.

## Hardware Required

- Soldered Inkplate 13
- USB cable
- Stable WiFi connection

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate13**
**WiFi Configuration → Enter your SSID and password**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

Display shows the raw HTML response from example.com.

## Notes

- `display.enableDithering(true)` is required for correct color rendering.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
