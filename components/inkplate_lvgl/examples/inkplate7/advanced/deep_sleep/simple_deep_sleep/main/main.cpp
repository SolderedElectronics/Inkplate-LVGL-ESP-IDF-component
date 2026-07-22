/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       Simple deep sleep example for Soldered Inkplate 7 with LVGL.
 *
 * @details     Puts the device into deep sleep, wakes it up every 30 seconds via
 *              the timer, and displays the wakeup count on the e-paper screen.
 *              The counter is preserved across deep sleep cycles using RTC memory.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 7
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 7, USB cable
 * - Extra:      None
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate7
 *
 * How to use:
 * 1) Build and flash to Inkplate 7.
 * 2) Device displays wake count, then sleeps for 30 seconds.
 * 3) On each wakeup the count increments and the screen refreshes.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 */

#include "sdkconfig.h"

#ifndef CONFIG_INKPLATE_BOARD_INKPLATE7
#error                                                                         \
    "Wrong board selection for this example, please select Inkplate7 in the boards menu."
#endif

#include "Inkplate.h"
#include "esp_sleep.h"

#define uS_TO_S_FACTOR 1000000ULL
#define TIME_TO_SLEEP  30

RTC_DATA_ATTR int wakeCount = 0;

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);
    display.enableDithering(true);

    wakeCount++;

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    char labelText[64];
    snprintf(labelText, sizeof(labelText), "Wake count: %d", wakeCount);

    lv_obj_t *label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, labelText);
    lv_obj_set_style_text_color(label, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_48, 0);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    lv_refr_now(lv_display_get_default());
    display.display();

    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    esp_deep_sleep_start();
}
