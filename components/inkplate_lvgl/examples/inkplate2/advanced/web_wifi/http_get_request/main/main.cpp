/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       HTTP GET request example for Soldered Inkplate 2 with LVGL.
 *
 * @details     Connects to WiFi, fetches a webpage via HTTP GET, and displays
 *              the raw response body on the e-paper screen. This example does
 *              not parse HTML; it prints the raw content as received.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 2
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 2, USB cable
 * - Extra:      Stable WiFi connection
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate2
 * - Menuconfig -> WiFi Configuration -> Enter your SSID and password
 *
 * How to use:
 * 1) Set WiFi credentials via idf.py menuconfig.
 * 2) Build and flash to Inkplate 2.
 * 3) Device fetches content from example.com and displays it.
 *
 * Expected output:
 * - Display shows raw HTTP response text from example.com.
 * - Serial monitor logs connection status and byte count.
 *
 * Notes:
 * - Raw HTML is displayed without parsing; long responses are truncated.
 * - For HTTPS, use wifi.downloadFileHTTPS() instead of wifi.downloadFile().
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
#include <string.h>

static const char *TAG = "HTTP_GET";

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

    WiFi wifi;
    if (wifi.begin() != ESP_OK || !wifi.waitForConnect(10000)) {
        ESP_LOGE(TAG, "WiFi connection failed");
        lv_label_set_text(label, "WiFi failed!");
        lv_refr_now(lv_display_get_default());
        display.display();
        return;
    }
    ESP_LOGI(TAG, "Connected to WiFi!");

    int32_t len = 32768;
    uint8_t *data = wifi.downloadFile("http://example.com/index.html", &len);

    if (!data || len <= 0) {
        ESP_LOGE(TAG, "HTTP GET failed");
        lv_label_set_text(label, "HTTP GET failed!");
        lv_refr_now(lv_display_get_default());
        display.display();
        return;
    }

    data[len] = '\0';
    ESP_LOGI(TAG, "Received %ld bytes", len);

    lv_obj_clean(screen);

    lv_obj_t *responseLabel = lv_label_create(screen);
    lv_obj_set_style_text_color(responseLabel, lv_color_black(), 0);
    lv_obj_set_style_text_font(responseLabel, &lv_font_montserrat_8, 0);
    lv_obj_set_width(responseLabel, E_INK_HEIGHT);
    lv_label_set_text(responseLabel, (char *)data);
    lv_obj_align(responseLabel, LV_ALIGN_TOP_LEFT, 5, 5);

    lv_refr_now(lv_display_get_default());
    display.display();

    free(data);
}
