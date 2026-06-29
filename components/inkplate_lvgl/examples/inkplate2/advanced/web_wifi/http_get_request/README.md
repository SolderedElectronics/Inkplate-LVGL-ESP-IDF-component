# HTTP GET Request

Fetch a webpage via HTTP GET and display the response on Soldered Inkplate 2 using LVGL.

## Overview

Connects to WiFi, fetches a webpage via HTTP GET, and displays the raw response body on the e-paper screen. This example does not parse HTML; it prints the raw content as received.

## Hardware Required

- Soldered Inkplate 2
- USB cable
- Stable WiFi connection

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

- Display shows raw HTTP response text from example.com.
- Serial monitor logs connection status and byte count.

## Notes

- Raw HTML is displayed without parsing; long responses are truncated.
- For HTTPS, use `wifi.downloadFileHTTPS()` instead of `wifi.downloadFile()`.
- `lv_refr_now()` renders the LVGL scene into the framebuffer.
- `display.display()` must be called to update the physical e-paper panel.

## Resources

- Docs: https://docs.soldered.com/inkplate
- Support: https://forum.soldered.com/
- Image tool: https://tools.soldered.com/tools/image-converter/
