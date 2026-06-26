/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       WiFi basics example for Soldered Inkplate 6.
 *
 * @details     Connects to a WiFi network using credentials set via menuconfig
 *              and displays the connection result on the e-paper screen.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 6, USB cable
 * - Extra:      Stable WiFi network
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate6
 * - Menuconfig -> WiFi Configuration -> Enter your SSID and password
 *
 * How to use:
 * 1) Set WiFi credentials via idf.py menuconfig.
 * 2) Build and flash to Inkplate 6.
 * 3) Display shows "Connected!" on success or "WiFi failed!" on failure.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 */

#include "sdkconfig.h"

#ifndef CONFIG_INKPLATE_BOARD_INKPLATE6
#error                                                                         \
    "Wrong board selection for this example, please select Inkplate6 in the boards menu."
#endif

#include "Inkplate.h"
#include "WiFi.h"
#include "esp_log.h"

static const char *TAG = "WIFI_BASICS";

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
    } else {
        ESP_LOGI(TAG, "Connected to WiFi!");
        lv_label_set_text(label, "Connected!");
    }

    lv_obj_center(label);
    lv_refr_now(lv_display_get_default());
    display.display();
}
