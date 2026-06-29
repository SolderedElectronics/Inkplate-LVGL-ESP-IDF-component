/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       Black & white screen split example for Soldered Inkplate 6Flick with LVGL.
 *
 * @details     Divides the display into a black left half and a white right half
 *              using LVGL objects in 1-bit black and white display mode.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6Flick
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 6Flick, USB cable
 * - Extra:      None
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate6 Flick
 *
 * How to use:
 * 1) Build and flash to Inkplate 6Flick.
 * 2) Left half of screen shows black, right half shows white.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 */

#include "sdkconfig.h"

#ifndef CONFIG_INKPLATE_BOARD_INKPLATE6FLICK
#error \
    "Wrong board selection for this example, please select Inkplate6Flick in the boards menu."
#endif

#include "Inkplate.h"

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);
    display.setDisplayMode(BLACK_AND_WHITE);
    display.frontlight.setState(false);

    lv_display_t *d = lv_display_get_default();
    int w = lv_display_get_horizontal_resolution(d);
    int h = lv_display_get_vertical_resolution(d);
    int half = w / 2;

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    lv_obj_t *left = lv_obj_create(lv_screen_active());
    lv_obj_remove_flag(left, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(left, half, h);
    lv_obj_set_pos(left, 0, 0);
    lv_obj_set_style_border_width(left, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(left, lv_color_hex(0x000000), LV_PART_MAIN);

    lv_obj_t *right = lv_obj_create(lv_screen_active());
    lv_obj_remove_flag(right, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(right, w - half, h);
    lv_obj_set_pos(right, half, 0);
    lv_obj_set_style_border_width(right, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(right, lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    lv_refr_now(lv_display_get_default());
    display.display();
}
