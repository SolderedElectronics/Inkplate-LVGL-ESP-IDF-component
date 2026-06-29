/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       HTTP GET request example for Soldered Inkplate 5.
 *
 * @details     Connects to WiFi, performs an HTTP GET to httpbin.org/get, and
 *              displays the raw JSON response body on the e-paper screen.
 *              httpbin.org/get returns a JSON object with request info (IP,
 *              headers, etc.) — useful for verifying network connectivity.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 5
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 5, USB cable
 * - Extra:      Stable WiFi connection
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate5
 * - Menuconfig -> WiFi Configuration -> Enter your SSID and password
 *
 * How to use:
 * 1) Set WiFi credentials via idf.py menuconfig.
 * 2) Build and flash to Inkplate 5.
 * 3) Display shows the raw JSON response from httpbin.org/get.
 *
 * Notes:
 * - Response is displayed as raw text without parsing.
 * - Long responses are truncated to fit the display buffer.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 */

#include "sdkconfig.h"

#ifndef CONFIG_INKPLATE_BOARD_INKPLATE5
#error                                                                         \
    "Wrong board selection for this example, please select Inkplate5 in the boards menu."
#endif

#include "Inkplate.h"
#include "WiFi.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "HTTP_GET";

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);
    display.setDisplayMode(BLACK_AND_WHITE);

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    lv_obj_t *label = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(label, lv_color_hex(0x000000), 0);
    lv_label_set_text(label, "Connecting to WiFi...");
    lv_obj_center(label);

    lv_refr_now(lv_display_get_default());
    display.display();

    WiFi wifi;
    bool connected = false;
    if (wifi.begin() == ESP_OK) {
        for (int attempt = 0; attempt < 3 && !connected; attempt++) {
            connected = wifi.waitForConnect(15000);
        }
    }

    if (!connected) {
        ESP_LOGE(TAG, "WiFi connection failed");
        lv_label_set_text(label, "WiFi failed!");
        lv_obj_center(label);
        lv_refr_now(lv_display_get_default());
        display.display();
        return;
    }
    ESP_LOGI(TAG, "Connected to WiFi!");

    lv_label_set_text(label, "Fetching data...");
    lv_obj_center(label);
    lv_refr_now(lv_display_get_default());
    display.display();

    int32_t len = 4096;
    uint8_t *data = wifi.downloadFile("http://httpbin.org/get", &len);

    if (!data || len <= 0) {
        ESP_LOGE(TAG, "HTTP GET failed");
        lv_label_set_text(label, "HTTP GET failed!");
        lv_obj_center(label);
        lv_refr_now(lv_display_get_default());
        display.display();
        return;
    }

    data[len] = '\0';
    ESP_LOGI(TAG, "Received %ld bytes", len);

    lv_obj_clean(lv_screen_active());

    lv_obj_t *responseLabel = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(responseLabel, lv_color_hex(0x000000), 0);
    lv_obj_set_width(responseLabel, E_INK_WIDTH - 10);
    lv_label_set_long_mode(responseLabel, LV_LABEL_LONG_WRAP);
    lv_label_set_text(responseLabel, (char *)data);
    lv_obj_align(responseLabel, LV_ALIGN_TOP_LEFT, 5, 5);

    lv_refr_now(lv_display_get_default());
    display.display();

    free(data);
}
