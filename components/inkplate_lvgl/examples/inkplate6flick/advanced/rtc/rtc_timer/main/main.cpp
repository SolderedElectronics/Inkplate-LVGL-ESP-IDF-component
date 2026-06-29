/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       RTC countdown timer clock example for Soldered Inkplate 6Flick.
 *
 * @details     Uses the PCF85063A countdown timer to fire once after 60 seconds.
 *              The display updates every second using fast partial refresh.
 *              A full refresh is performed every 60 partials to reduce ghosting.
 *              "Timer!" is appended to the label when the countdown completes.
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
 * 2) Display shows live time; "Timer!" appears after 60 s.
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
#include <string.h>

#define FULL_REFRESH_EVERY 60

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

    char buf[160];
    snprintf(buf, sizeof(buf),
             "%02d:%02d:%02d\n%s, %02d/%02d/%04d",
             hour, minute, second, weekdayNames[weekday], day, month, year);

    if (display.rtc.checkTimerFlag()) {
        display.rtc.clearTimerFlag();
        strlcat(buf, "\n\nTimer!", sizeof(buf));
    }

    lv_label_set_text(label, buf);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_PARTIAL);
    display.setDisplayMode(BLACK_AND_WHITE);
    display.frontlight.setState(false);

    // Always set time on boot — no deep sleep involved, no reason to preserve state
    struct tm t = {};
    t.tm_hour = 14;
    t.tm_min  = 30;
    t.tm_sec  = 0;
    t.tm_mday = 12;
    t.tm_wday = 3;    // Wednesday (0=Sun)
    t.tm_mon  = 11;   // November (1-12)
    t.tm_year = 2025; // Full year
    display.rtc.setTime(t);

    // One-shot countdown: 1 Hz source, 60 s, interrupt enabled, not periodic
    display.rtc.setTimer(RTC_TIMER_CLOCK_1HZ, 60, true, false);

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    lv_obj_t *label = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(label, lv_color_hex(0x000000), LV_PART_MAIN);

    updateTimeLabel(label, display);
    lv_refr_now(lv_display_get_default());
    display.display();

    int partialCount = 0;

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));

        updateTimeLabel(lv_obj_get_child(lv_screen_active(), 0), display);
        lv_refr_now(lv_display_get_default());

        if (++partialCount >= FULL_REFRESH_EVERY) {
            display.display();
            partialCount = 0;
        } else {
            display.partialUpdate();
        }
    }
}
