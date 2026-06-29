/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       Temperature reading example for Soldered Inkplate 6.
 *
 * @details     Reads the panel temperature from the TPS65186 PMIC thermistor
 *              and displays it on the e-ink panel. The display refreshes every
 *              10 seconds using fast partial update.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 6, USB cable
 * - Extra:      None
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate6
 *
 * How to use:
 * 1) Build and flash to Inkplate 6.
 * 2) Display shows panel temperature in Celsius, updating every 10 s.
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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_PARTIAL);
    display.setDisplayMode(BLACK_AND_WHITE);

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    lv_obj_t *label = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(label, lv_color_hex(0x000000), 0);
    lv_obj_center(label);

    int8_t temp = display.readTemperature();
    char buf[32];
    snprintf(buf, sizeof(buf), "%d C", temp);
    lv_label_set_text(label, buf);

    lv_refr_now(lv_display_get_default());
    display.display();

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10000));

        temp = display.readTemperature();
        snprintf(buf, sizeof(buf), "%d C", temp);
        lv_label_set_text(lv_obj_get_child(lv_screen_active(), 0), buf);
        lv_obj_center(lv_obj_get_child(lv_screen_active(), 0));

        lv_refr_now(lv_display_get_default());
        display.partialUpdate();
    }
}
