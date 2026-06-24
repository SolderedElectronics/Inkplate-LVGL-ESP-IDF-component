/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       Color palette example for Soldered Inkplate 6Color with LVGL.
 *
 * @details     Demonstrates all 7 native colors of the Inkplate 6Color e-paper
 *              display (black, white, green, yellow, blue, red, orange) by
 *              rendering seven vertical rectangles side by side using LVGL.
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
 * 2) After initialization, seven vertical colored rectangles appear on screen.
 *
 * Expected output:
 * - Seven vertical rectangles: black, white, green, yellow, blue, red, orange.
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
    "Wrong board selection for this example, please select Inkplate6 Color in the boards menu."
#endif

#include "Inkplate.h"

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);
    display.enableDithering(false);

    lv_obj_t *screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    const int NUM_RECTS = 7;
    const int RECT_WIDTH = E_INK_WIDTH / NUM_RECTS;

    uint32_t colors[NUM_RECTS] = {
        0x000000, // Black
        0xFFFFFF, // White
        0x00FF00, // Green
        0xFFFF00, // Yellow
        0x0000FF, // Blue
        0xFF0000, // Red
        0xFF8000, // Orange
    };

    for (int i = 0; i < NUM_RECTS; i++) {
        lv_obj_t *rect = lv_obj_create(screen);
        lv_obj_set_size(rect, RECT_WIDTH, E_INK_HEIGHT);
        lv_obj_set_pos(rect, i * RECT_WIDTH, 0);
        lv_obj_set_style_bg_color(rect, lv_color_hex(colors[i]), LV_PART_MAIN);
        lv_obj_set_style_border_width(rect, 0, LV_PART_MAIN);
        lv_obj_set_style_radius(rect, 0, LV_PART_MAIN);
    }

    lv_refr_now(lv_display_get_default()); // Render LVGL scene into framebuffer
    display.display();                     // Push framebuffer to e-paper panel
}
