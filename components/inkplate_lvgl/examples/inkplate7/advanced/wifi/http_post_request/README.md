# HTTP POST Request

Send an HTTP POST request and display the result on Soldered Inkplate 7 using LVGL.

## Overview

Connects to WiFi and sends an HTTP POST request with a JSON payload to webhook.site. The HTTP status code is shown on the e-paper display. Visit webhook.site to see the incoming request.

## Hardware Required

- Soldered Inkplate 7
- USB cable
- Stable WiFi connection

## Setup

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate7**
**WiFi Configuration → Enter your SSID and password**

Edit `WEBHOOK_PATH` in `main.cpp` with your unique webhook.site path (e.g. `/abcd-1234`).

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

Display shows "POST sent!" and the HTTP status code, or an error message on failure.

## Notes

- Get a free unique webhook URL at https://webhook.site.
- `display.enableDithering(true)` is required for correct color rendering.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
