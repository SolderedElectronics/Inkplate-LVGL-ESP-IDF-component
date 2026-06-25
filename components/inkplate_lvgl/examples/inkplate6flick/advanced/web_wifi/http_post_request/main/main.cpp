/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       HTTP POST request example for Soldered Inkplate 6Flick.
 *
 * @details     Connects to WiFi and sends an HTTP POST request with a JSON
 *              payload to httpbin.org/post, which echoes the request back.
 *              The HTTP status code and a snippet of the response are shown
 *              on the e-paper display.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6Flick
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 6Flick, USB cable
 * - Extra:      Stable WiFi connection
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate6 Flick
 * - Menuconfig -> WiFi Configuration -> Enter your SSID and password
 *
 * How to use:
 * 1) Set WiFi credentials via idf.py menuconfig.
 * 2) Build and flash to Inkplate 6Flick.
 * 3) Display shows HTTP status code and response snippet.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 */

#include "sdkconfig.h"

#ifndef CONFIG_INKPLATE_BOARD_INKPLATE6FLICK
#error \
    "Wrong board selection for this example, please select Inkplate6Flick in the boards menu."
#endif

#include "Inkplate.h"
#include "WiFi.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>

static const char *TAG = "HTTP_POST";

#define POST_URL     "http://httpbin.org/post"
#define RESP_BUF_LEN 2048

static char s_respBuf[RESP_BUF_LEN];
static int  s_respLen = 0;

static esp_err_t httpEventHandler(esp_http_client_event_t *evt) {
    if (evt->event_id == HTTP_EVENT_ON_DATA && evt->data_len > 0) {
        int remaining = RESP_BUF_LEN - s_respLen - 1;
        if (remaining > 0) {
            int copy = evt->data_len < remaining ? evt->data_len : remaining;
            memcpy(s_respBuf + s_respLen, evt->data, copy);
            s_respLen += copy;
            s_respBuf[s_respLen] = '\0';
        }
    }
    return ESP_OK;
}

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);
    display.setDisplayMode(BLACK_AND_WHITE);
    display.frontlight.setState(false);

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

    lv_label_set_text(label, "Sending POST...");
    lv_obj_center(label);
    lv_refr_now(lv_display_get_default());
    display.display();

    const char *postData = "{\"device\":\"Inkplate6Flick\",\"status\":\"Hello from Inkplate!\"}";

    esp_http_client_config_t config = {};
    config.url        = POST_URL;
    config.method     = HTTP_METHOD_POST;
    config.timeout_ms = 10000;
    config.event_handler = httpEventHandler;

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, postData, strlen(postData));

    esp_err_t ret = esp_http_client_perform(client);

    char resultText[128];
    if (ret == ESP_OK) {
        int status = esp_http_client_get_status_code(client);
        ESP_LOGI(TAG, "POST sent, status: %d", status);
        snprintf(resultText, sizeof(resultText), "POST sent!\nStatus: %d", status);
    } else {
        ESP_LOGE(TAG, "POST failed: %s", esp_err_to_name(ret));
        snprintf(resultText, sizeof(resultText), "POST failed:\n%s", esp_err_to_name(ret));
    }

    esp_http_client_cleanup(client);

    lv_obj_clean(lv_screen_active());

    lv_obj_t *statusLabel = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(statusLabel, lv_color_hex(0x000000), 0);
    lv_label_set_text(statusLabel, resultText);
    lv_obj_align(statusLabel, LV_ALIGN_TOP_MID, 0, 10);

    if (s_respLen > 0) {
        lv_obj_t *respLabel = lv_label_create(lv_screen_active());
        lv_obj_set_style_text_color(respLabel, lv_color_hex(0x000000), 0);
        lv_obj_set_width(respLabel, E_INK_WIDTH - 10);
        lv_label_set_long_mode(respLabel, LV_LABEL_LONG_WRAP);
        lv_label_set_text(respLabel, s_respBuf);
        lv_obj_align(respLabel, LV_ALIGN_TOP_LEFT, 5, 60);
    }

    lv_refr_now(lv_display_get_default());
    display.display();
}
