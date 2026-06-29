# HTTP POST Request

Send an HTTP POST request with a JSON payload on Soldered Inkplate 2 using LVGL.

## Overview

Connects to WiFi and sends an HTTP POST request with a JSON payload to webhook.site. The POST result is shown on the display. Visit https://webhook.site to get a free unique URL for testing.

## Hardware Required

- Soldered Inkplate 2
- USB cable
- Stable WiFi connection
- webhook.site URL

## Setup

1. Visit https://webhook.site and copy your unique path (e.g. `/abc-123`).
2. Paste it into `WEBHOOK_PATH` in `main.cpp`.

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate2**
**WiFi Configuration → Enter your SSID and password**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

- Display shows POST result (status code or error).
- Serial monitor logs connection and POST status.
- Incoming POST visible on webhook.site.

## Notes

- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
