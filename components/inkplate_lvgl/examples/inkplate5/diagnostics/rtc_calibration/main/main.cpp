/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       RTC calibration example for Soldered Inkplate 5 with LVGL.
 *
 * @details     Initialises the PCF85063A RTC with default calibration settings
 *              and prints HH:MM:SS to the serial console every second.
 *              Adjust setClockOffset() to fine-tune the crystal frequency.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 5
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 5, USB cable
 * - Extra:      None
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate5
 *
 * How to use:
 * 1) Build and flash to Inkplate 5.
 * 2) Open a serial terminal at 115200 baud.
 * 3) RTC starts at 00:00:00 and prints time every second.
 * 4) Compare with a reference clock; adjust setClockOffset() as needed.
 *
 * Notes:
 * - setInternalCapacitor(true) selects the 12.5 pF internal load capacitor.
 * - setClockOffset(mode, value): mode 1 = correction every 4 minutes;
 *   value is two's complement offset (-63 to +63).
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 */

#include "sdkconfig.h"

#ifndef CONFIG_INKPLATE_BOARD_INKPLATE5
#error                                                                         \
    "Wrong board selection for this example, please select Inkplate5 in the boards menu."
#endif

#include "Inkplate.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <time.h>

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);
    display.setDisplayMode(BLACK_AND_WHITE);

    lv_obj_t *label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "RTC calibration\n\nOpen Serial Monitor\n115200 baud.");
    lv_obj_set_style_text_color(label, lv_color_hex(0x000000), 0);
    lv_obj_center(label);

    lv_refr_now(lv_display_get_default());
    display.display();

    // 12.5 pF internal load capacitor (true = 12.5 pF, false = 7 pF)
    display.rtc.setInternalCapacitor(true);
    // Correction every 4 minutes (mode=1), offset -63 — adjust to calibrate
    display.rtc.setClockOffset(1, -63);

    struct tm startTime = {};
    display.rtc.setTime(startTime);

    lv_label_set_text(label, "RTC started.\nTracking time...\nCheck Serial Monitor.");
    lv_obj_center(label);

    lv_refr_now(lv_display_get_default());
    display.display();

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));

        uint8_t h = display.rtc.getHour();
        uint8_t m = display.rtc.getMinute();
        uint8_t s = display.rtc.getSecond();

        printf("%02d:%02d:%02d\n", h, m, s);
    }
}
