/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       Battery voltage reading example for Soldered Inkplate 5.
 *
 * @details     Reads the LiPo battery voltage via the ESP32 ADC and displays
 *              it on the e-ink panel. The display refreshes every 10 seconds
 *              using fast partial update.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 5
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 5, USB cable, LiPo battery (optional)
 * - Extra:      None
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate5
 *
 * How to use:
 * 1) Build and flash to Inkplate 5.
 * 2) Display shows battery voltage in volts, updating every 10 s.
 *
 * Notes:
 * - Without a battery connected the reading shows the USB supply (~4.2 V).
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

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_PARTIAL);
    display.setDisplayMode(BLACK_AND_WHITE);

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    lv_obj_t *label = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(label, lv_color_hex(0x000000), 0);
    lv_obj_center(label);

    double voltage = display.readBattery();
    char buf[32];
    snprintf(buf, sizeof(buf), "%.2f V", voltage);
    lv_label_set_text(label, buf);

    lv_refr_now(lv_display_get_default());
    display.display();

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10000));

        voltage = display.readBattery();
        snprintf(buf, sizeof(buf), "%.2f V", voltage);
        lv_label_set_text(lv_obj_get_child(lv_screen_active(), 0), buf);
        lv_obj_center(lv_obj_get_child(lv_screen_active(), 0));

        lv_refr_now(lv_display_get_default());
        display.partialUpdate();
    }
}
