/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       Deep sleep with button wake example for Soldered Inkplate 10.
 *
 * @details     Wakes from deep sleep either on timer (30 s) or when the
 *              WAKE button (GPIO 36) is pressed (active-low).
 *              The wake reason and boot count are shown on the display.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 10
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 10, USB cable
 * - Extra:      None
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate10
 *
 * How to use:
 * 1) Build and flash to Inkplate 10.
 * 2) Display shows boot count and wake reason, then enters deep sleep.
 * 3) Press WAKE button or wait 30 s to wake again.
 *
 * Notes:
 * - GPIO 36 is the WAKE button pin (active-low).
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 */

#include "sdkconfig.h"

#ifndef CONFIG_INKPLATE_BOARD_INKPLATE10
#error \
    "Wrong board selection for this example, please select Inkplate10 in the boards menu."
#endif

#include "Inkplate.h"
#include "driver/gpio.h"
#include "esp_sleep.h"

#define uS_TO_S_FACTOR 1000000ULL
#define TIME_TO_SLEEP  30

RTC_DATA_ATTR int bootCount = 0;

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);
    display.setDisplayMode(BLACK_AND_WHITE);

    bootCount++;

    esp_sleep_wakeup_cause_t wakeupReason = esp_sleep_get_wakeup_cause();
    const char *reasonText = "Wakeup: Unknown";
    switch (wakeupReason) {
        case ESP_SLEEP_WAKEUP_EXT0:  reasonText = "Wakeup: Button"; break;
        case ESP_SLEEP_WAKEUP_TIMER: reasonText = "Wakeup: Timer";  break;
        default: break;
    }

    char labelText[128];
    snprintf(labelText, sizeof(labelText), "Boot count: %d\n%s", bootCount, reasonText);

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    lv_obj_t *label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, labelText);
    lv_obj_set_style_text_color(label, lv_color_hex(0x000000), 0);
    lv_obj_center(label);

    lv_refr_now(lv_display_get_default());
    display.display();

    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    // GPIO 36 = WAKE button, active-low
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_36, 0);
    esp_deep_sleep_start();
}
