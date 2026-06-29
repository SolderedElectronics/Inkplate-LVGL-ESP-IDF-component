/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       Simple RTC clock example for Soldered Inkplate 6Flick with LVGL.
 *
 * @details     Sets an initial date and time on the PCF85063A RTC and refreshes
 *              the display every minute with the current time and date.
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
 * 2) Display shows current date and time, refreshing every minute.
 *
 * Notes:
 * - setTime(struct tm) uses tm_mon = 1-12 and tm_year = full year (e.g. 2025).
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

static void updateTimeLabel(lv_obj_t *label, Inkplate &display) {
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

    lv_label_set_text(label, timeText);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);
    display.setDisplayMode(BLACK_AND_WHITE);
    display.frontlight.setState(false);

    display.rtc.reset();

    // Wednesday, 12 November 2025, 14:30:00
    // Note: setTime(struct tm) uses tm_mon = 1-12, tm_year = full year
    struct tm t = {};
    t.tm_hour = 14;
    t.tm_min  = 30;
    t.tm_sec  = 0;
    t.tm_mday = 12;
    t.tm_wday = 3;    // Wednesday (0=Sun)
    t.tm_mon  = 11;   // November (1-12)
    t.tm_year = 2025; // Full year
    display.rtc.setTime(t);

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    lv_obj_t *label = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(label, lv_color_hex(0x000000), LV_PART_MAIN);

    updateTimeLabel(label, display);
    lv_refr_now(lv_display_get_default());
    display.display();

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(60000));
        updateTimeLabel(lv_obj_get_child(lv_screen_active(), 0), display);
        lv_refr_now(lv_display_get_default());
        display.display();
    }
}
