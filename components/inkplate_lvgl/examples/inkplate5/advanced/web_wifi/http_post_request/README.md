# HTTP POST Request

Send an HTTP POST request and display the response on Soldered Inkplate 5 using LVGL.

## Overview

Connects to WiFi and sends an HTTP POST request with a JSON payload to httpbin.org/post, which echoes the request back. The HTTP status code and a snippet of the response are shown on the e-paper display.

## Hardware Required

- Soldered Inkplate 5
- USB cable
- Stable WiFi connection

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate5**
**WiFi Configuration → Enter your SSID and password**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

Display shows HTTP status code and response snippet.

## Notes

- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
