/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       Simple deep sleep example for Soldered Inkplate 6Flick with LVGL.
 *
 * @details     Puts Inkplate into deep sleep and wakes every 30 seconds.
 *              A counter retained in RTC memory tracks the number of wakes
 *              and is displayed on the e-ink panel on each boot.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6Flick
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 6Flick, USB cable
 * - Extra:      None
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate6 Flick
 *
 * How to use:
 * 1) Build and flash to Inkplate 6Flick.
 * 2) Display shows wake count and enters deep sleep every 30 s.
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
#include "esp_sleep.h"

#define uS_TO_S_FACTOR 1000000ULL
#define TIME_TO_SLEEP  30

RTC_DATA_ATTR int wakeCount = 0;

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);
    display.setDisplayMode(BLACK_AND_WHITE);
    display.frontlight.setState(false);

    wakeCount++;

    char labelText[64];
    snprintf(labelText, sizeof(labelText), "Wake count: %d", wakeCount);

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    lv_obj_t *label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, labelText);
    lv_obj_set_style_text_color(label, lv_color_hex(0x000000), 0);
    lv_obj_center(label);

    lv_refr_now(lv_display_get_default());
    display.display();

    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    esp_deep_sleep_start();
}
