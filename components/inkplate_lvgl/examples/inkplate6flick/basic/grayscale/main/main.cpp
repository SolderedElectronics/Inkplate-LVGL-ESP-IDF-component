/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       Grayscale bars example for Soldered Inkplate 6Flick with LVGL.
 *
 * @details     Displays 8 vertical bars showing 8 shades of gray (0–7) using
 *              LVGL objects in 3-bit grayscale display mode.
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
 * 2) Screen shows 8 vertical gray bars from black (0) to white (7).
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
#include <stdio.h>

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);
    display.setDisplayMode(GRAYSCALE);
    display.frontlight.setState(false);

    lv_display_t *d = lv_display_get_default();
    int w = lv_display_get_horizontal_resolution(d);
    int h = lv_display_get_vertical_resolution(d);
    int colW = w / 8;

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    for (int i = 0; i < 8; i++) {
        uint8_t shade = i * 36;
        lv_obj_t *rect = lv_obj_create(lv_screen_active());
        lv_obj_remove_flag(rect, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_size(rect, colW, h);
        lv_obj_set_pos(rect, i * colW, 0);
        lv_obj_set_style_border_width(rect, 0, LV_PART_MAIN);
        lv_obj_set_style_bg_color(rect, lv_color_make(shade, shade, shade), LV_PART_MAIN);
    }

    for (int i = 0; i < 8; i++) {
        char txt[4];
        snprintf(txt, sizeof(txt), "%d", i);
        lv_obj_t *label = lv_label_create(lv_screen_active());
        lv_label_set_text(label, txt);
        lv_obj_set_style_text_color(label, lv_color_hex(0x000000), 0);
        lv_obj_align(label, LV_ALIGN_TOP_LEFT, i * colW + colW / 2 - 5, 10);
    }

    lv_refr_now(lv_display_get_default());
    display.display();
}
