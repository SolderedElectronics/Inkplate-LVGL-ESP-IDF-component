/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       HTTP POST request example for Soldered Inkplate 2 with LVGL.
 *
 * @details     Connects to WiFi and sends an HTTP POST request with a JSON
 *              payload to webhook.site. The POST result is shown on the display.
 *              Visit https://webhook.site to get a free unique URL for testing.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 2
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 2, USB cable
 * - Extra:      Stable WiFi connection, webhook.site URL
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate2
 * - Menuconfig -> WiFi Configuration -> Enter your SSID and password
 * - Set WEBHOOK_PATH below to your unique webhook.site path
 *
 * How to use:
 * 1) Visit https://webhook.site and copy your unique path (e.g. /abc-123).
 * 2) Paste it into WEBHOOK_PATH below.
 * 3) Set WiFi credentials via idf.py menuconfig.
 * 4) Build and flash to Inkplate 2.
 * 5) Watch incoming POST on webhook.site.
 *
 * Expected output:
 * - Display shows POST result (status code or error).
 * - Serial monitor logs connection and POST status.
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
#include "esp_http_client.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>

static const char *TAG = "HTTP_POST";

#define WEBHOOK_HOST "webhook.site"
#define WEBHOOK_PATH "/YOUR-UNIQUE-WEBHOOK-ID"

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

    char url[128];
    snprintf(url, sizeof(url), "http://%s%s", WEBHOOK_HOST, WEBHOOK_PATH);

    const char *postData = "{\"device\":\"Inkplate2\", \"status\":\"Hello from Inkplate!\"}";

    esp_http_client_config_t config = {};
    config.url = url;
    config.method = HTTP_METHOD_POST;
    config.timeout_ms = 10000;

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, postData, strlen(postData));

    esp_err_t ret = esp_http_client_perform(client);

    char resultText[64];
    if (ret == ESP_OK) {
        int status = esp_http_client_get_status_code(client);
        ESP_LOGI(TAG, "POST sent, status: %d", status);
        snprintf(resultText, sizeof(resultText), "POST sent!\nStatus: %d", status);
    } else {
        ESP_LOGE(TAG, "POST failed: %s", esp_err_to_name(ret));
        snprintf(resultText, sizeof(resultText), "POST failed:\n%s", esp_err_to_name(ret));
    }

    esp_http_client_cleanup(client);

    lv_label_set_text(label, resultText);
    lv_refr_now(lv_display_get_default());
    display.display();
}
