/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       RTC timer + live clock example for Soldered Inkplate 5.
 *
 * @details     Sets an RTC countdown timer for 60 seconds. Displays current
 *              time updating every second via partial refresh. When the timer
 *              fires, "Timer!" is shown on the display.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 5
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 5, USB cable, CR2032 battery (optional)
 * - Extra:      None
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate5
 *
 * How to use:
 * 1) Build and flash to Inkplate 5.
 * 2) Display shows a live clock; "Timer!" appears after 60 s.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 */

#include "sdkconfig.h"

#ifndef CONFIG_INKPLATE_BOARD_INKPLATE5
#error \
    "Wrong board selection for this example, please select Inkplate5 in the boards menu."
#endif

#include "Inkplate.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include <time.h>

#define FULL_REFRESH_EVERY 60

static const char *weekdayNames[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

static void updateLabel(Inkplate &display, lv_obj_t *label) {
    uint8_t hour    = display.rtc.getHour();
    uint8_t minute  = display.rtc.getMinute();
    uint8_t second  = display.rtc.getSecond();
    uint8_t day     = display.rtc.getDay();
    uint8_t weekday = display.rtc.getWeekday();
    uint8_t month   = display.rtc.getMonth();
    uint16_t year   = display.rtc.getYear();

    char buf[160];
    snprintf(buf, sizeof(buf), "%02d:%02d:%02d\n%s, %02d/%02d/%04d",
             hour, minute, second, weekdayNames[weekday], day, month, year);

    if (display.rtc.checkTimerFlag()) {
        display.rtc.clearTimerFlag();
        strlcat(buf, "\n\nTimer!", sizeof(buf));
    }

    lv_label_set_text(label, buf);
    lv_obj_center(label);
}

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_PARTIAL);
    display.setDisplayMode(BLACK_AND_WHITE);

    if (!display.rtc.isSet()) {
        struct tm t = {};
        t.tm_hour = 14;
        t.tm_min  = 30;
        t.tm_sec  = 0;
        t.tm_mday = 12;
        t.tm_wday = 3;
        t.tm_mon  = 11;
        t.tm_year = 2025;
        display.rtc.setTime(t);
    }

    // 60-second one-shot timer, interrupt enabled, pulse mode off
    display.rtc.setTimer(RTC_TIMER_CLOCK_1HZ, 60, true, false);

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    lv_obj_t *label = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(label, lv_color_hex(0x000000), LV_PART_MAIN);

    updateLabel(display, label);

    lv_refr_now(lv_display_get_default());
    display.display(); // initial full refresh

    int partialCount = 0;
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        updateLabel(display, label);
        lv_refr_now(lv_display_get_default());

        if (++partialCount >= FULL_REFRESH_EVERY) {
            display.display();
            partialCount = 0;
        } else {
            display.partialUpdate();
        }
    }
}
