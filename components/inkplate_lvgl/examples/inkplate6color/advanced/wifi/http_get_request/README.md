# HTTP GET Request

Fetch a webpage via HTTP GET and display the response on Soldered Inkplate 6Color using LVGL.

## Overview

Connects to WiFi, sends an HTTP GET request to example.com, and displays the raw response body on the e-paper screen. No HTML parsing is performed — the raw text/HTML is shown as-is.

## Hardware Required

- Soldered Inkplate 6Color
- USB cable
- Stable WiFi connection

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate6 Color**
**WiFi Configuration → Enter your SSID and password**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

Device fetches example.com and displays the raw response.

## Notes

- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
