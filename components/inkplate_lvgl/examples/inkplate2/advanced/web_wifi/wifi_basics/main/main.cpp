/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       WiFi basics example for Soldered Inkplate 2 with LVGL.
 *
 * @details     Demonstrates how to connect the Inkplate 2 to a WiFi network
 *              and display the connection status on the e-paper screen.
 *              WiFi credentials are set via menuconfig.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 2
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 2, USB cable
 * - Extra:      Stable WiFi network
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate2
 * - Menuconfig -> WiFi Configuration -> Enter your SSID and password
 *
 * How to use:
 * 1) Set WiFi credentials via idf.py menuconfig.
 * 2) Build and flash to Inkplate 2.
 * 3) Device connects to WiFi and shows status on display.
 *
 * Expected output:
 * - Display shows "Connected!" on success or "WiFi failed!" on failure.
 * - Serial monitor shows connection progress.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 */

#include "sdkconfig.h"

#ifndef CONFIG_INKPLATE_BOARD_INKPLATE2
#error                                                                         \
    "Wrong board selection for this example, please select Inkplate2 in the boards menu."
#endif

#include "Inkplate.h"
#include "WiFi.h"
#include "esp_log.h"

static const char *TAG = "WIFI_BASICS";

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);
    display.clearDisplay();
    display.enableDithering(false);

    lv_obj_t *screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    lv_obj_t *label = lv_label_create(screen);
    lv_obj_set_style_text_color(label, lv_color_black(), 0);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_8, 0);
    lv_obj_center(label);

    ESP_LOGI(TAG, "Connecting to WiFi...");
    WiFi wifi;
    if (wifi.begin() != ESP_OK || !wifi.waitForConnect(10000)) {
        ESP_LOGE(TAG, "WiFi connection failed");
        lv_label_set_text(label, "WiFi failed!");
    } else {
        ESP_LOGI(TAG, "Connected to WiFi!");
        lv_label_set_text(label, "Connected!");
    }

    lv_refr_now(lv_display_get_default());
    display.display();
}
