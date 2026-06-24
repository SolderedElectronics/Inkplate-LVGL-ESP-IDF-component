/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       RTC timer example for Soldered Inkplate 6Color with LVGL.
 *
 * @details     Configures the RTC countdown timer for 60 seconds at 1 Hz.
 *              When the timer fires, "Timer!" is appended to the displayed
 *              time string. The display refreshes every 60 seconds.
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
 * 2) Time updates every 60 s; "Timer!" appears when the countdown finishes.
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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static void updateTimeLabel(Inkplate &display, lv_obj_t *label) {
    display.rtc.getRtcData();

    const char *wdayNames[] = {
        "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

    char labelText[160];
    snprintf(labelText, sizeof(labelText),
             "%02d:%02d:%02d\n%s, %02d/%02d/%04d",
             display.rtc.getHour(), display.rtc.getMinute(), display.rtc.getSecond(),
             wdayNames[display.rtc.getWeekday()],
             display.rtc.getDay(), display.rtc.getMonth(), display.rtc.getYear());

    if (display.rtc.checkTimerFlag()) {
        display.rtc.clearTimerFlag();
        display.rtc.disableTimer();
        strcat(labelText, "\n\nTimer!");
    }

    lv_label_set_text(label, labelText);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);
    display.rtc.reset();

    display.rtc.setTime(14, 30, 0);
    display.rtc.setDate(3, 12, 11, 2025);

    display.rtc.timerSet(RTC::TIMER_CLOCK_1HZ, 60, true, false);

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    lv_obj_t *label = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(label, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_26, 0);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    updateTimeLabel(display, label);
    lv_refr_now(lv_display_get_default());
    display.display();

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(60000));
        updateTimeLabel(display, label);
        lv_refr_now(lv_display_get_default());
        display.display();
    }
}
