# HTTP POST Request

Send an HTTP POST request to webhook.site on Soldered Inkplate 6Color using LVGL.

## Overview

Connects to WiFi, sends a JSON HTTP POST request to webhook.site, and displays the result (HTTP status or error) on the e-paper screen. Visit https://webhook.site to get a free unique URL for testing.

## Hardware Required

- Soldered Inkplate 6Color
- USB cable
- Stable WiFi connection
- webhook.site URL

## Setup

1. Visit https://webhook.site and copy your unique path (e.g. `/abcd-1234`).
2. Paste it into `WEBHOOK_PATH` in `main.cpp`.

Run `idf.py menuconfig` and navigate to:
**Inkplate Boards → Inkplate6 Color**
**WiFi Configuration → Enter your SSID and password**

## Build and Flash

```
idf.py build
idf.py -p PORT flash monitor
```

## Expected Output

- Display shows POST result (HTTP status or error).
- Incoming POST visible on webhook.site.

## Notes

- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
