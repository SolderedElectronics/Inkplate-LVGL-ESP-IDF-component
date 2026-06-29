# HTTP GET Request

Fetch a webpage via HTTP GET and display the response on Soldered Inkplate 4 using LVGL.

## Overview

Connects to WiFi, performs an HTTP GET to httpbin.org/get, and displays the raw JSON response body on the e-paper screen. httpbin.org/get returns a JSON object with request info (IP, headers, etc.) — useful for verifying network connectivity. Response is displayed as raw text without parsing.

## Hardware Required

- Soldered Inkplate 4
- USB cable
- Stable WiFi connection

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate4**
**WiFi Configuration → Enter your SSID and password**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

Display shows the raw JSON response from httpbin.org/get.

## Notes

- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
