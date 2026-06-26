/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       RTC alarm deep sleep example for Soldered Inkplate 6.
 *
 * @details     Sets an RTC alarm 60 seconds ahead, displays current time, then
 *              enters deep sleep. The RTC alarm interrupt (GPIO 39) wakes the
 *              device and the display is refreshed with the new time.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 6, USB cable, CR2032 battery (optional)
 * - Extra:      None
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate6
 *
 * How to use:
 * 1) Build and flash to Inkplate 6.
 * 2) Display shows current time and enters deep sleep.
 * 3) Device wakes every 60 s via RTC alarm.
 *
 * Notes:
 * - The RTC time is set once (if not already set) and persists across deep sleep.
 * - GPIO 39 is the RTC INT pin (active-low alarm output).
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 */

#include "sdkconfig.h"

#ifndef CONFIG_INKPLATE_BOARD_INKPLATE6
#error \
    "Wrong board selection for this example, please select Inkplate6 in the boards menu."
#endif

#include "Inkplate.h"
#include "esp_sleep.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <time.h>

static const char *weekdayNames[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);
    display.setDisplayMode(BLACK_AND_WHITE);

    display.rtc.clearAlarmFlag();

    // Only skip setTime on a genuine RTC alarm wakeup — the RTC is still running.
    // Any other wakeup (power-on, reset, USB flash) must set the time.
    if (esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_EXT0) {
        // Wednesday, 12 November 2025, 13:30:00
        // Note: setTime(struct tm) uses tm_mon = 1-12, tm_year = full year
        struct tm t = {};
        t.tm_hour = 13;
        t.tm_min  = 30;
        t.tm_sec  = 0;
        t.tm_mday = 12;
        t.tm_wday = 3;    // Wednesday (0=Sun)
        t.tm_mon  = 11;   // November (1-12)
        t.tm_year = 2025; // Full year
        display.rtc.setTime(t);
    }

    uint8_t hour    = display.rtc.getHour();
    uint8_t minute  = display.rtc.getMinute();
    uint8_t second  = display.rtc.getSecond();
    uint8_t day     = display.rtc.getDay();
    uint8_t weekday = display.rtc.getWeekday();
    uint8_t month   = display.rtc.getMonth();
    uint16_t year   = display.rtc.getYear();

    char timeText[128];
    snprintf(timeText, sizeof(timeText),
             "%s, %02d.%02d.%04d\n%02d:%02d:%02d",
             weekdayNames[weekday], day, month, year, hour, minute, second);

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    lv_obj_t *label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, timeText);
    lv_obj_set_style_text_color(label, lv_color_hex(0x000000), 0);
    lv_obj_center(label);

    lv_refr_now(lv_display_get_default());
    display.display();

    // Set alarm 60 s from now
    time_t epoch;
    display.rtc.getTime(&epoch);
    display.rtc.setAlarmEpoch(epoch + 60);

    // GPIO 39 = RTC INT pin, active-low alarm output
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_39, 0);
    esp_deep_sleep_start();
}
