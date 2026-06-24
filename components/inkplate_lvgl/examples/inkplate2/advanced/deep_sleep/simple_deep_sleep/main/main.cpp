/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       Simple deep sleep example for Soldered Inkplate 2 with LVGL.
 *
 * @details     Demonstrates ESP32 deep sleep with timer wakeup. On each wake,
 *              the wake count is incremented (persisted via RTC_DATA_ATTR),
 *              displayed on the e-paper screen, then the device returns to sleep.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 2
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 2, USB cable
 * - Extra:      None
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate2
 *
 * How to use:
 * 1) Build and flash to Inkplate 2.
 * 2) Device displays wake count, then sleeps for TIME_TO_SLEEP seconds.
 * 3) On each wakeup the counter increments and the display updates.
 *
 * Expected output:
 * - Display shows "Wake count: N" updating every TIME_TO_SLEEP seconds.
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
#include "esp_log.h"
#include "esp_sleep.h"
#include <stdio.h>

#define TIME_TO_SLEEP 30ULL

static const char *TAG = "DEEP_SLEEP";

RTC_DATA_ATTR int wakeCount = 0;

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);

    wakeCount++;
    ESP_LOGI(TAG, "Wake count: %d", wakeCount);

    lv_obj_t *screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    char labelText[64];
    snprintf(labelText, sizeof(labelText), "Wake count: %d", wakeCount);

    lv_obj_t *label = lv_label_create(screen);
    lv_label_set_text(label, labelText);
    lv_obj_set_style_text_color(label, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_10, 0);
    lv_obj_center(label);

    lv_refr_now(lv_display_get_default());
    display.display();

    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * 1000000ULL);
    esp_deep_sleep_start();
}
