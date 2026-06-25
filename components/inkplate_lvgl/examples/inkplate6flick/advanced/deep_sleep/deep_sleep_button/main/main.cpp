/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       Deep sleep with button wake example for Soldered Inkplate 6Flick.
 *
 * @details     Wakes from deep sleep either on timer (30 s) or when the
 *              touchscreen interrupt line (GPIO 36) goes low (touch event).
 *              The wake reason and boot count are shown on the display.
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
 * 2) Display shows boot count and wake reason, then enters deep sleep.
 * 3) Touch the screen or wait 30 s to wake again.
 *
 * Notes:
 * - GPIO 36 is the touchscreen INT pin; it goes low on any touch event.
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
#include "driver/gpio.h"
#include "esp_sleep.h"

#define uS_TO_S_FACTOR 1000000ULL
#define TIME_TO_SLEEP  30

RTC_DATA_ATTR int bootCount = 0;

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);
    display.setDisplayMode(BLACK_AND_WHITE);
    display.frontlight.setState(false);

    bootCount++;

    esp_sleep_wakeup_cause_t wakeupReason = esp_sleep_get_wakeup_cause();
    const char *reasonText = "Wakeup: Unknown";
    switch (wakeupReason) {
        case ESP_SLEEP_WAKEUP_EXT0:  reasonText = "Wakeup: Touch";  break;
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
    // GPIO 36 = touchscreen INT: goes low on touch event (active-low)
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_36, 0);
    esp_deep_sleep_start();
}
