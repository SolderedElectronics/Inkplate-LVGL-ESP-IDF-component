/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       RTC alarm deep sleep example for Soldered Inkplate 6Color with LVGL.
 *
 * @details     Sets the RTC date/time, then configures an alarm to fire 60 seconds
 *              later. The device enters deep sleep and wakes when the RTC alarm
 *              triggers via the INT pin (GPIO39). Current time is displayed on
 *              wakeup.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6Color
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 6Color, USB cable
 * - Extra:      None
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate6 Color
 *
 * How to use:
 * 1) Build and flash to Inkplate 6Color.
 * 2) Device displays current time, sleeps, wakes every 60 s via RTC alarm.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 */

#include "sdkconfig.h"

#ifndef CONFIG_INKPLATE_BOARD_INKPLATE6COLOR
#error \
    "Wrong board selection for this example, please select Inkplate6Color in the boards menu."
#endif

#include "Inkplate.h"
#include "esp_sleep.h"
#include <stdio.h>

static char timeText[128];

static void buildCurrentTimeString(Inkplate &display) {
    display.rtc.getRtcData();

    const char *weekdayNames[] = {
        "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

    snprintf(timeText, sizeof(timeText),
             "%s, %02d.%02d.%04d\n%02d:%02d:%02d",
             weekdayNames[display.rtc.getWeekday()],
             display.rtc.getDay(), display.rtc.getMonth(), display.rtc.getYear(),
             display.rtc.getHour(), display.rtc.getMinute(), display.rtc.getSecond());
}

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);
    display.rtc.clearAlarmFlag();

    if (!display.rtc.isSet()) {
        display.rtc.setTime(13, 30, 0);
        display.rtc.setDate(3, 12, 11, 2025);
    }

    buildCurrentTimeString(display);

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    lv_obj_t *label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, timeText);
    lv_obj_set_style_text_color(label, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_28, 0);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    lv_refr_now(lv_display_get_default());
    display.display();

    display.rtc.setAlarmEpoch(display.rtc.getEpoch() + 60, RTC_ALARM_MATCH_DHHMMSS);

    esp_sleep_enable_ext0_wakeup(GPIO_NUM_39, 0);
    esp_deep_sleep_start();
}
