/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       Deep sleep with button wakeup example for Soldered Inkplate 13 with LVGL.
 *
 * @details     Demonstrates waking the ESP32-S3 from deep sleep via two sources:
 *              a hardware button on GPIO18 (EXT0 wakeup) or a 30-second timer.
 *              The boot counter and wakeup reason are displayed on screen.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 13
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 13, USB cable
 * - Extra:      None
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate13
 *
 * How to use:
 * 1) Build and flash to Inkplate 13.
 * 2) Device shows boot count and wakeup reason, then sleeps.
 * 3) Press the WAKEUP button (GPIO18) or wait 30 s to wake it again.
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
#include "esp_sleep.h"

#define uS_TO_S_FACTOR 1000000ULL
#define TIME_TO_SLEEP  30

RTC_DATA_ATTR int bootCount = 0;

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);
    display.enableDithering(true);

    bootCount++;

    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
    const char *reasonText = nullptr;
    switch (wakeup_reason) {
        case ESP_SLEEP_WAKEUP_EXT0:  reasonText = "Wakeup: Button";         break;
        case ESP_SLEEP_WAKEUP_TIMER: reasonText = "Wakeup: Timer";          break;
        default:                     reasonText = "Wakeup: Power on / Reset"; break;
    }

    char labelText[128];
    snprintf(labelText, sizeof(labelText), "Boot count: %d\n%s", bootCount, reasonText);

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    lv_obj_t *label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, labelText);
    lv_obj_set_style_text_color(label, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_26, 0);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    lv_refr_now(lv_display_get_default());
    display.display();

    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_18, 0);
    esp_deep_sleep_start();
}
