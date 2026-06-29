/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       HTTP GET request example for Soldered Inkplate 13 with LVGL.
 *
 * @details     Connects to WiFi, sends an HTTP GET request to example.com, and
 *              displays the raw response body on the e-paper screen.
 *              No HTML parsing is performed — the raw text/HTML is shown as-is.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 13
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 13, USB cable
 * - Extra:      Stable WiFi connection
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate13
 * - Menuconfig -> WiFi Configuration -> Enter your SSID and password
 *
 * How to use:
 * 1) Set WiFi credentials via menuconfig.
 * 2) Build and flash to Inkplate 13.
 * 3) Device fetches example.com and displays the raw response.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 */

#include "sdkconfig.h"

#ifndef CONFIG_INKPLATE_BOARD_INKPLATE13
#error                                                                         \
    "Wrong board selection for this example, please select Inkplate13 in the boards menu."
#endif

#include "Inkplate.h"
#include "WiFi.h"
#include <string.h>

#define MAX_RESPONSE_LEN 4096

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);
    display.enableDithering(true);

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    lv_obj_t *label = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(label, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_14, 0);
    lv_obj_set_width(label, 1160);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_label_set_text(label, "Connecting to WiFi...");
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 20, 20);

    lv_refr_now(lv_display_get_default());
    display.display();

    WiFi wifi;
    if (wifi.begin() != ESP_OK || !wifi.waitForConnect(10000)) {
        lv_label_set_text(label, "WiFi connection failed!");
        lv_obj_align(label, LV_ALIGN_TOP_LEFT, 20, 20);
        lv_refr_now(lv_display_get_default());
        display.display();
        return;
    }

    lv_label_set_text(label, "Connected! Fetching...");
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 20, 20);
    lv_refr_now(lv_display_get_default());
    display.display();

    int32_t len = MAX_RESPONSE_LEN - 1;
    uint8_t *data = wifi.downloadFile("http://example.com/index.html", &len);

    if (!data || len <= 0) {
        lv_label_set_text(label, "HTTP GET failed!");
        lv_obj_align(label, LV_ALIGN_TOP_LEFT, 20, 20);
        lv_refr_now(lv_display_get_default());
        display.display();
        return;
    }

    if (len >= MAX_RESPONSE_LEN) len = MAX_RESPONSE_LEN - 1;
    data[len] = '\0';

    lv_label_set_text(label, (char *)data);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 20, 20);
    lv_refr_now(lv_display_get_default());
    display.display();

    free(data);
}
