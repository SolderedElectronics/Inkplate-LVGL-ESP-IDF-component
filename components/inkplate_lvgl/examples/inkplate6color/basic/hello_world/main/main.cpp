/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       Basic "Hello World" example for Soldered Inkplate 6Color with LVGL.
 *
 * @details     Demonstrates the most basic usage of the Inkplate 6Color with
 *              LVGL by initializing the display and rendering a "Hello World!"
 *              label on the e-paper screen using the LVGL graphics library.
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
 * 2) After initialization, "Hello World!" appears on the display.
 *
 * Expected output:
 * - The text "Hello World!" centered on the Inkplate screen.
 *
 * Notes:
 * - lv_refr_now() renders the LVGL scene into the framebuffer.
 * - display.display() must be called to update the physical e-paper panel.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 */

#include "sdkconfig.h"

#ifndef CONFIG_INKPLATE_BOARD_INKPLATE6COLOR
#error                                                                         \
    "Wrong board selection for this example, please select Inkplate6Color in the boards menu."
#endif

#include "Inkplate.h"

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);

    lv_obj_t *screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    lv_obj_t *label = lv_label_create(screen);
    lv_label_set_text(label, "Hello World!");
    lv_obj_center(label);

    lv_refr_now(lv_display_get_default()); // Render LVGL scene into framebuffer
    display.display();                     // Push framebuffer to e-paper panel
}
