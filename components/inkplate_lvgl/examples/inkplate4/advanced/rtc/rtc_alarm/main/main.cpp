/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       RTC alarm example for Soldered Inkplate 4.
 *
 * @details     Sets RTC time/date and an alarm 1 minute ahead. Displays current
 *              time every 60 s. When the alarm fires, "ALARM!" appears on screen.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 4
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 4, USB cable, CR2032 battery (optional)
 * - Extra:      None
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate4
 *
 * How to use:
 * 1) Build and flash to Inkplate 4.
 * 2) Display shows current time, "ALARM!" appears after ~1 minute.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 */

#include "sdkconfig.h"

#ifndef CONFIG_INKPLATE_BOARD_INKPLATE4
#error \
    "Wrong board selection for this example, please select Inkplate4 in the boards menu."
#endif

#include "Inkplate.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <time.h>

static const char *weekdayNames[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

static void updateLabels(Inkplate &display, lv_obj_t *timeLabel, lv_obj_t *alarmLabel) {
    uint8_t hour    = display.rtc.getHour();
    uint8_t minute  = display.rtc.getMinute();
    uint8_t second  = display.rtc.getSecond();
    uint8_t day     = display.rtc.getDay();
    uint8_t weekday = display.rtc.getWeekday();
    uint8_t month   = display.rtc.getMonth();
    uint16_t year   = display.rtc.getYear();

    char buf[128];
    snprintf(buf, sizeof(buf), "%02d:%02d:%02d\n%s, %02d/%02d/%04d",
             hour, minute, second, weekdayNames[weekday], day, month, year);
    lv_label_set_text(timeLabel, buf);
    lv_obj_align(timeLabel, LV_ALIGN_CENTER, 0, -30);

    if (display.rtc.checkAlarmFlag()) {
        display.rtc.clearAlarmFlag();
        lv_label_set_text(alarmLabel, "ALARM!");
    } else {
        lv_label_set_text(alarmLabel, "");
    }
    lv_obj_align(alarmLabel, LV_ALIGN_CENTER, 0, 60);
}

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);
    display.setDisplayMode(BLACK_AND_WHITE);

    display.rtc.reset();

    // Wednesday, 12 November 2025, 14:30:00
    struct tm t = {};
    t.tm_hour = 14;
    t.tm_min  = 30;
    t.tm_sec  = 0;
    t.tm_mday = 12;
    t.tm_wday = 3;
    t.tm_mon  = 11;
    t.tm_year = 2025;
    display.rtc.setTime(t);

    // Alarm at 14:31:00 on day 12, weekday 3
    display.rtc.setAlarm(0, 31, 14, 12, 3);

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    lv_obj_t *timeLabel = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(timeLabel, lv_color_hex(0x000000), LV_PART_MAIN);

    lv_obj_t *alarmLabel = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(alarmLabel, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_label_set_text(alarmLabel, "");

    updateLabels(display, timeLabel, alarmLabel);

    lv_refr_now(lv_display_get_default());
    display.display();

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(60000));
        updateLabels(display, timeLabel, alarmLabel);
        lv_refr_now(lv_display_get_default());
        display.display();
    }
}
