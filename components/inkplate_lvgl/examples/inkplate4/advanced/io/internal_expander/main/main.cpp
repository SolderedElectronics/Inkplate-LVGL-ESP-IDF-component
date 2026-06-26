/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       Internal IO expander example for Soldered Inkplate 4.
 *
 * @details     Blinks an LED connected to the internal IO expander pin P1-7
 *              (GPB7 = IO_NUM_B7). Only Port B pins (GPB1–GPB7, IO_NUM_B1–B7)
 *              are safe to use; Port A and GPB0 are reserved for the display.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 4
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 4, 330 Ohm resistor, LED
 *               LED anode -> resistor -> IO Expander 1 pin P1-7 (GPB7)
 *               LED cathode -> GND
 * - Extra:      None
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate4
 *
 * How to use:
 * 1) Wire LED + 330 Ohm resistor to P1-7 on the IO Expander 1 header.
 * 2) Build and flash to Inkplate 4.
 * 3) LED blinks once per second.
 *
 * IMPORTANT:
 * - DO NOT use P0-0 to P0-7 (GPA0–GPA7) or P1-0 (GPB0) — reserved for
 *   the e-paper panel and PMIC. Using them may permanently damage the screen.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 */

#include "sdkconfig.h"

#ifndef CONFIG_INKPLATE_BOARD_INKPLATE4
#error \
    "Wrong board selection for this example, please select Inkplate4 in the boards menu."
#endif

#include "Inkplate.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);
    display.setDisplayMode(BLACK_AND_WHITE);

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    lv_obj_t *label = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(label, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_label_set_text(label, "Internal IO expander\nBlinking LED on P1-7 (GPB7)");
    lv_obj_center(label);

    lv_refr_now(lv_display_get_default());
    display.display();

    // GPB7 = IO_NUM_B7 — safe to use on internal expander
    expander1.setDirection(IO_NUM_B7, IO_MODE_OUTPUT);

    while (1) {
        expander1.setLevel(IO_NUM_B7, 0); // LED off
        vTaskDelay(pdMS_TO_TICKS(1000));
        expander1.setLevel(IO_NUM_B7, 1); // LED on
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
