/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       RTC alarm interrupt example for Soldered Inkplate 5.
 *
 * @details     Sets an RTC alarm 60 seconds after a known epoch. The RTC INT
 *              pin (GPIO 39) triggers a GPIO interrupt which sets a flag.
 *              When the alarm fires the display shows "ALARM!".
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
 * 2) Display shows current time; "ALARM!" appears ~60 s after boot.
 *
 * Notes:
 * - GPIO 39 is the RTC INT pin (active-low alarm output).
 * - GPIO_INTR_LOW_LEVEL used instead of edge — level is reliable even if the
 *   falling edge occurs while display is refreshing (interrupts temporarily off).
 * - ISR disables its own interrupt; main loop re-enables after clearAlarmFlag().
 * - Task notification used for ISR-to-task signalling (safer than volatile flag
 *   on dual-core ESP32).
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
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <time.h>

static void IRAM_ATTR alarmISR(void *arg) {
    gpio_intr_disable(GPIO_NUM_39);
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR((TaskHandle_t)arg, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static const char *weekdayNames[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

static void updateLabels(Inkplate &display, lv_obj_t *timeLabel) {
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
    t.tm_wday = 3;    // Wednesday (0=Sun)
    t.tm_mon  = 11;   // November (1-12)
    t.tm_year = 2025;
    display.rtc.setTime(t);

    // Alarm at 14:31:00 — match only on hour/min/sec, not day/weekday
    display.rtc.setAlarm(0, 31, 14);

    TaskHandle_t mainTask = xTaskGetCurrentTaskHandle();

    // Configure GPIO 39 as input with low-level interrupt (RTC INT active-low)
    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = (1ULL << GPIO_NUM_39);
    io_conf.mode         = GPIO_MODE_INPUT;
    io_conf.pull_up_en   = GPIO_PULLUP_DISABLE;
    io_conf.intr_type    = GPIO_INTR_LOW_LEVEL;
    gpio_config(&io_conf);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_NUM_39, alarmISR, (void *)mainTask);

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    lv_obj_t *timeLabel = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(timeLabel, lv_color_hex(0x000000), LV_PART_MAIN);

    lv_obj_t *alarmLabel = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(alarmLabel, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_label_set_text(alarmLabel, "");

    updateLabels(display, timeLabel);

    lv_refr_now(lv_display_get_default());
    display.display();

    while (1) {
        uint32_t notified = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(60000));
        updateLabels(display, timeLabel);
        if (notified) {
            display.rtc.clearAlarmFlag();
            gpio_intr_enable(GPIO_NUM_39);
            lv_label_set_text(alarmLabel, "ALARM!");
        } else {
            lv_label_set_text(alarmLabel, "");
        }
        lv_obj_align(alarmLabel, LV_ALIGN_CENTER, 0, 60);
        lv_refr_now(lv_display_get_default());
        display.display();
    }
}
