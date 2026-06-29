/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       HTTP POST request example for Soldered Inkplate 13 with LVGL.
 *
 * @details     Connects to WiFi, sends a JSON HTTP POST request to webhook.site,
 *              and displays the result (HTTP status or error) on the e-paper screen.
 *              Visit https://webhook.site to get a free unique URL for testing.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 13
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 13, USB cable
 * - Extra:      Stable WiFi connection, webhook.site URL
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate13
 * - Menuconfig -> WiFi Configuration -> Enter your SSID and password
 * - Set WEBHOOK_PATH below to your unique webhook.site path
 *
 * How to use:
 * 1) Visit https://webhook.site and copy your unique path (e.g. /abcd-1234).
 * 2) Paste it into WEBHOOK_PATH below.
 * 3) Set WiFi credentials via menuconfig.
 * 4) Build and flash. Check webhook.site to see the incoming POST request.
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
#include "esp_http_client.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>

static const char *TAG = "HTTP_POST";

#define WEBHOOK_HOST "webhook.site"
#define WEBHOOK_PATH "/YOUR-UNIQUE-WEBHOOK-ID"

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);
    display.enableDithering(true);

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    lv_obj_t *label = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(label, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_28, 0);
    lv_label_set_text(label, "HTTP POST example\nUsing webhook.site");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    lv_refr_now(lv_display_get_default());
    display.display();

    WiFi wifi;
    if (wifi.begin() != ESP_OK || !wifi.waitForConnect(10000)) {
        lv_label_set_text(label, "WiFi failed!");
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
        lv_refr_now(lv_display_get_default());
        display.display();
        return;
    }

    char url[128];
    snprintf(url, sizeof(url), "http://%s%s", WEBHOOK_HOST, WEBHOOK_PATH);

    const char *postData =
        "{\"device\":\"Inkplate13\",\"status\":\"Hello from Inkplate!\"}";

    esp_http_client_config_t config = {};
    config.url         = url;
    config.method      = HTTP_METHOD_POST;
    config.timeout_ms  = 10000;

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, postData, strlen(postData));

    esp_err_t ret = esp_http_client_perform(client);

    char resultText[64];
    if (ret == ESP_OK) {
        int status = esp_http_client_get_status_code(client);
        snprintf(resultText, sizeof(resultText), "POST sent!\nHTTP %d", status);
        ESP_LOGI(TAG, "POST OK, status=%d", status);
    } else {
        snprintf(resultText, sizeof(resultText), "POST failed:\n%s", esp_err_to_name(ret));
        ESP_LOGE(TAG, "POST failed: %s", esp_err_to_name(ret));
    }

    esp_http_client_cleanup(client);

    lv_label_set_text(label, resultText);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    lv_refr_now(lv_display_get_default());
    display.display();
}
