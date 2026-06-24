/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       RTC calibration example for Soldered Inkplate 6Color with LVGL.
 *
 * @details     Demonstrates how to calibrate the PCF85063A RTC for improved
 *              time accuracy. Sets the internal capacitor and clock offset, then
 *              tracks time after a wake button press. Time is printed to serial
 *              every second; display shows status prompts.
 *
 *              If the RTC drifts, adjust the offset value passed to
 *              setClockOffset() — negative values slow the clock, positive speed
 *              it up. Compare against a reference clock over several minutes.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6Color
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 6Color, USB cable
 * - Extra:      Serial monitor at 115200 baud
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate6 Color
 *
 * How to use:
 * 1) Open serial monitor at 115200 baud.
 * 2) Build and flash to Inkplate 6Color.
 * 3) Press the wake button when prompted on the display.
 * 4) Observe HH:MM:SS printed every second on serial monitor.
 * 5) Compare against a reference clock; adjust setClockOffset() as needed.
 *
 * Expected output:
 * - Display shows status prompts.
 * - Serial monitor prints 00:00:00, 00:00:01, ... after button press.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 */

#include "sdkconfig.h"

#ifndef CONFIG_INKPLATE_BOARD_INKPLATE6COLOR
#error                                                                         \
    "Wrong board selection for this example, please select Inkplate6 Color in the boards menu."
#endif

#include "Inkplate.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include <time.h>

static const char *TAG = "RTC_CAL";

#define WAKE_BUTTON_PIN GPIO_NUM_36

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);

    lv_obj_t *screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    lv_obj_t *mainLabel = lv_label_create(screen);
    lv_obj_set_style_text_font(mainLabel, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(mainLabel, lv_color_black(), 0);

    lv_label_set_text(mainLabel, "RTC calibration\n\nOpen Serial Monitor\nat 115200 baud.");
    lv_obj_center(mainLabel);
    lv_refr_now(lv_display_get_default());
    display.display();

    // GPIO36 is input-only on ESP32; no internal pull available, board has external pull-up
    gpio_config_t io_conf = {};
    io_conf.intr_type    = GPIO_INTR_DISABLE;
    io_conf.mode         = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << WAKE_BUTTON_PIN);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en   = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    // 12.5 pF internal capacitor (true = 12.5pF, false = 7pF)
    display.rtc.setInternalCapacitor(true);

    // Clock offset: mode 1 = correction every 4 minutes, value in two's complement
    display.rtc.setClockOffset(1, -63);

    ESP_LOGI(TAG, "Press the wake-up button to start RTC");

    lv_label_set_text(mainLabel, "RTC calibration\n\nPress Wake button\nto start.");
    lv_obj_center(mainLabel);
    lv_refr_now(lv_display_get_default());
    display.display();

    // Wait for button press (LOW = pressed, board has external pull-up)
    while (gpio_get_level(WAKE_BUTTON_PIN) == 1) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    // Start RTC at 00:00:00
    struct tm startTime = {};
    startTime.tm_hour = 0;
    startTime.tm_min  = 0;
    startTime.tm_sec  = 0;
    display.rtc.setTime(startTime);

    lv_label_set_text(mainLabel, "RTC started.\nTracking time...\nSee Serial Monitor\nat 115200 baud.");
    lv_obj_center(mainLabel);
    lv_refr_now(lv_display_get_default());
    display.display();

    uint64_t lastPrint = esp_timer_get_time() / 1000ULL;

    while (true) {
        uint64_t now = esp_timer_get_time() / 1000ULL;
        if (now - lastPrint >= 1000) {
            uint8_t h = display.rtc.getHour();
            uint8_t m = display.rtc.getMinute();
            uint8_t s = display.rtc.getSecond();
            printf("%02d:%02d:%02d\n", h, m, s);
            lastPrint = now;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
