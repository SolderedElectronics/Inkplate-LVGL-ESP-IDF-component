/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       Burn-in cleaning example for Soldered Inkplate 10 with LVGL.
 *
 * @details     Runs multiple full-panel cleaning cycles to reduce burn-in
 *              visible on the e-ink panel. Set CLEAR_CYCLES and CYCLES_DELAY
 *              as needed, then flash and wait for the "Cleaning Done!" message.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 10
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 10, USB cable
 * - Extra:      None
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate10
 *
 * How to use:
 * 1) Build and flash to Inkplate 10.
 * 2) Wait for all cleaning cycles to complete.
 * 3) "Cleaning Done!" appears on screen when finished.
 *
 * Notes:
 * - CLEAR_CYCLES sets the number of cleaning iterations.
 * - CYCLES_DELAY sets the delay in milliseconds between each cycle.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 */

#include "sdkconfig.h"

#ifndef CONFIG_INKPLATE_BOARD_INKPLATE10
#error \
    "Wrong board selection for this example, please select Inkplate10 in the boards menu."
#endif

#include "Inkplate.h"

#define CLEAR_CYCLES 20
#define CYCLES_DELAY 5000

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);
    display.setDisplayMode(BLACK_AND_WHITE);

    lv_obj_t *label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Running burn-in clean...\nPlease wait.");
    lv_obj_set_style_text_color(label, lv_color_hex(0x000000), 0);
    lv_obj_center(label);

    lv_refr_now(lv_display_get_default());
    display.display();

    display.cleanBurnIn(CLEAR_CYCLES, CYCLES_DELAY);

    lv_obj_del(label);

    lv_obj_t *doneLabel = lv_label_create(lv_screen_active());
    lv_label_set_text(doneLabel, "Cleaning Done!");
    lv_obj_set_style_text_color(doneLabel, lv_color_hex(0x000000), 0);
    lv_obj_center(doneLabel);

    lv_refr_now(lv_display_get_default());
    display.display();
}
