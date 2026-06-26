/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       External IO expander example for Soldered Inkplate 4.
 *
 * @details     Blinks an LED connected to the external IO expander pin P1-7
 *              (GPB7 = IO_NUM_B7). All pins on the external expander are free
 *              to use — nothing is internally connected.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 4
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 4, 330 Ohm resistor, LED
 *               LED anode -> resistor -> IO Expander 2 pin P1-7 (GPB7)
 *               LED cathode -> GND
 * - Extra:      None
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate4
 *
 * How to use:
 * 1) Wire LED + 330 Ohm resistor to P1-7 on the IO Expander 2 header.
 * 2) Build and flash to Inkplate 4.
 * 3) LED blinks once per second.
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

// expander2 is defined in BoardCommon.cpp but not exported in any header
extern PCAL expander2;

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);
    display.setDisplayMode(BLACK_AND_WHITE);

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    lv_obj_t *label = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(label, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_label_set_text(label, "External IO expander\nBlinking LED on P1-7 (GPB7)");
    lv_obj_center(label);

    lv_refr_now(lv_display_get_default());
    display.display();

    // GPB7 = IO_NUM_B7
    expander2.setDirection(IO_NUM_B7, IO_MODE_OUTPUT);

    while (1) {
        expander2.setLevel(IO_NUM_B7, 0); // LED off
        vTaskDelay(pdMS_TO_TICKS(1000));
        expander2.setLevel(IO_NUM_B7, 1); // LED on
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
