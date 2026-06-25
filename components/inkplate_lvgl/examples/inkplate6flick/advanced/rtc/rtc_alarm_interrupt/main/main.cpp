/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       RTC alarm interrupt example for Soldered Inkplate 6Flick with LVGL.
 *
 * @details     Sets an epoch-based RTC time and schedules an alarm 60 seconds
 *              later. A GPIO ISR on pin 39 (RTC INT) sets a flag when the alarm
 *              fires. The display refreshes every minute; "ALARM!" is shown when
 *              the interrupt flag is detected.
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
 * 2) Display shows time; "ALARM!" appears 60 s after boot.
 *
 * Notes:
 * - GPIO 39 is the RTC INT pin (active-low, open-drain with external pull-up).
 *   The touchscreen driver installs the ISR service; this example adds GPIO 39
 *   as an additional handler without re-installing the service.
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
#include "esp_attr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <time.h>

static volatile bool alarmFlag = false;

static void IRAM_ATTR alarmISR(void *arg) {
    alarmFlag = true;
}

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

    if (alarmFlag) {
        alarmFlag = false;
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

    // Set time from epoch; Inkplate constructor has already installed the ISR service
    display.rtc.setTime((time_t)1762957188);

    time_t epoch;
    display.rtc.getTime(&epoch);
    display.rtc.setAlarmEpoch(epoch + 60);

    // GPIO 39: RTC INT pin, input with pull-up, falling edge
    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = (1ULL << GPIO_NUM_39);
    io_conf.mode         = GPIO_MODE_INPUT;
    io_conf.pull_up_en   = GPIO_PULLUP_ENABLE;
    io_conf.intr_type    = GPIO_INTR_NEGEDGE;
    gpio_config(&io_conf);
    // ISR service already installed by the touchscreen driver in the Inkplate constructor
    gpio_isr_handler_add(GPIO_NUM_39, alarmISR, NULL);

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
