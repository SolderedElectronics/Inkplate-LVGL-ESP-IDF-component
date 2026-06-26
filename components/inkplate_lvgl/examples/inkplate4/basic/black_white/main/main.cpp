/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       Black & white screen split example for Soldered Inkplate 4 with LVGL.
 *
 * @details     Demonstrates 1-bit (black & white) mode on Inkplate 4 by splitting
 *              the display into a black left half and a white right half using LVGL.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 4
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 4, USB cable
 * - Extra:      None
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate4
 *
 * How to use:
 * 1) Build and flash to Inkplate 4.
 * 2) Left half of the display turns black, right half stays white.
 *
 * Expected output:
 * - Screen split vertically: left half black, right half white.
 *
 * Notes:
 * - lv_refr_now() renders the LVGL scene into the framebuffer.
 * - display.display() must be called to update the physical e-paper panel.
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

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);
    display.setDisplayMode(BLACK_AND_WHITE);

    lv_obj_t *screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    lv_display_t *d = lv_display_get_default();
    int w = lv_display_get_horizontal_resolution(d);
    int h = lv_display_get_vertical_resolution(d);
    int half = w / 2;

    lv_obj_t *left = lv_obj_create(screen);
    lv_obj_remove_flag(left, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(left, half, h);
    lv_obj_set_pos(left, 0, 0);
    lv_obj_set_style_border_width(left, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(left, lv_color_black(), LV_PART_MAIN);

    lv_obj_t *right = lv_obj_create(screen);
    lv_obj_remove_flag(right, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(right, w - half, h);
    lv_obj_set_pos(right, half, 0);
    lv_obj_set_style_border_width(right, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(right, lv_color_white(), LV_PART_MAIN);

    lv_refr_now(lv_display_get_default()); // Render LVGL scene into framebuffer
    display.display();                     // Push framebuffer to e-paper panel
}
