/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       RTC alarm example for Soldered Inkplate 6Flick with LVGL.
 *
 * @details     Sets an initial time and a one-minute alarm on the PCF85063A RTC.
 *              The display refreshes every minute. When the alarm fires, an
 *              "ALARM!" message is shown alongside the current date and time.
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
 * 2) Display shows time; "ALARM!" appears when the alarm fires at 14:31.
 *
 * Notes:
 * - setAlarm(sec, min, hour, day, weekday) matches all five fields.
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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *weekdayNames[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

static void updateDisplay(lv_obj_t *timeLabel, lv_obj_t *alarmLabel, Inkplate &display) {
    uint8_t  hour    = display.rtc.getHour();
    uint8_t  minute  = display.rtc.getMinute();
    uint8_t  second  = display.rtc.getSecond();
    uint8_t  day     = display.rtc.getDay();
    uint8_t  weekday = display.rtc.getWeekday();
    uint8_t  month   = display.rtc.getMonth();
    uint16_t year    = display.rtc.getYear();

    char timeText[128];
    snprintf(timeText, sizeof(timeText),
             "%02d:%02d:%02d\n%s, %02d/%02d/%04d",
             hour, minute, second, weekdayNames[weekday], day, month, year);
    lv_label_set_text(timeLabel, timeText);
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
    display.frontlight.setState(false);

    display.rtc.reset();

    // Wednesday, 12 November 2025, 14:30:00
    struct tm t = {};
    t.tm_hour = 14;
    t.tm_min  = 30;
    t.tm_sec  = 0;
    t.tm_mday = 12;
    t.tm_wday = 3;    // Wednesday (0=Sun)
    t.tm_mon  = 11;   // November (1-12)
    t.tm_year = 2025; // Full year
    display.rtc.setTime(t);

    // Alarm at 14:31:00 on Wednesday 12th
    display.rtc.setAlarm(0, 31, 14, 12, 3);

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    lv_obj_t *timeLabel = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(timeLabel, lv_color_hex(0x000000), LV_PART_MAIN);

    lv_obj_t *alarmLabel = lv_label_create(lv_screen_active());
    lv_label_set_text(alarmLabel, "");
    lv_obj_set_style_text_color(alarmLabel, lv_color_hex(0x000000), LV_PART_MAIN);

    updateDisplay(timeLabel, alarmLabel, display);
    lv_refr_now(lv_display_get_default());
    display.display();

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(60000));
        updateDisplay(lv_obj_get_child(lv_screen_active(), 0),
                      lv_obj_get_child(lv_screen_active(), 1),
                      display);
        lv_refr_now(lv_display_get_default());
        display.display();
    }
}
