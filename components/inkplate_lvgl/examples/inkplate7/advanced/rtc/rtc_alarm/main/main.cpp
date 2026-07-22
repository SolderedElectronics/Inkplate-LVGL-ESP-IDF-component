/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       RTC alarm example for Soldered Inkplate 7 with LVGL.
 *
 * @details     Sets the RTC time, date, and an alarm 1 minute in the future.
 *              The display refreshes every 60 seconds. When the alarm fires,
 *              "ALARM!" is shown in green on screen.
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
 * 2) Time updates every 60 s; "ALARM!" appears when the alarm triggers.
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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <time.h>

static void updateTimeLabel(Inkplate &display, lv_obj_t *label, lv_obj_t *alarmLabel) {
    struct tm t = {};
    display.rtc.getTime(&t);

    const char *wdayNames[] = {
        "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

    char timeText[128];
    snprintf(timeText, sizeof(timeText),
             "%02d:%02d:%02d\n%s, %02d/%02d/%04d",
             t.tm_hour, t.tm_min, t.tm_sec,
             wdayNames[t.tm_wday],
             t.tm_mday, t.tm_mon, t.tm_year);

    lv_label_set_text(label, timeText);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, -30);

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
    display.enableDithering(true);
    display.rtc.reset();

    // Wednesday, 12 November 2025, 14:30:00
    struct tm t = {};
    t.tm_hour = 14; t.tm_min = 30; t.tm_sec = 0;
    t.tm_mday = 12; t.tm_wday = 3; t.tm_mon = 11; t.tm_year = 2025;
    display.rtc.setTime(t);

    // Alarm at 14:31:00
    display.rtc.setAlarm(0, 31, 14, 12, 3);

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    lv_obj_t *timeLabel = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(timeLabel, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_text_font(timeLabel, &lv_font_montserrat_26, 0);
    lv_obj_align(timeLabel, LV_ALIGN_CENTER, 0, -30);

    lv_obj_t *alarmLabel = lv_label_create(lv_screen_active());
    lv_label_set_text(alarmLabel, "");
    lv_obj_set_style_text_color(alarmLabel, lv_color_hex(0x00AA00), LV_PART_MAIN);
    lv_obj_set_style_text_font(alarmLabel, &lv_font_montserrat_48, 0);
    lv_obj_align(alarmLabel, LV_ALIGN_CENTER, 0, 60);

    updateTimeLabel(display, timeLabel, alarmLabel);
    lv_refr_now(lv_display_get_default());
    display.display();

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(60000));
        updateTimeLabel(display, timeLabel, alarmLabel);
        lv_refr_now(lv_display_get_default());
        display.display();
    }
}
