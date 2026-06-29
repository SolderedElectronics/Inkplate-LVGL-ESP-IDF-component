/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       Grayscale example for Soldered Inkplate 4 with LVGL.
 *
 * @details     Demonstrates 3-bit (8-level grayscale) mode on Inkplate 4 by drawing
 *              8 vertical bars with shades from black (0) to white (7).
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
 * 2) Eight vertical grayscale bars (0–7) appear across the display.
 *
 * Expected output:
 * - 8 vertical bars from black (leftmost) to white (rightmost), each labeled 0–7.
 *
 * Notes:
 * - lv_refr_now() renders the LVGL scene into the framebuffer.
 * - display.display() must be called to update the physical e-paper panel.
 * - Dithering is only supported in FULL render mode.
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
    display.setDisplayMode(GRAYSCALE);

    lv_obj_t *screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    lv_display_t *d = lv_display_get_default();
    int w = lv_display_get_horizontal_resolution(d);
    int h = lv_display_get_vertical_resolution(d);
    int colW = w / 8;

    for (int i = 0; i < 8; i++) {
        uint8_t shade = i * 36;
        lv_obj_t *rect = lv_obj_create(screen);
        lv_obj_remove_flag(rect, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_size(rect, colW, h);
        lv_obj_set_pos(rect, i * colW, 0);
        lv_obj_set_style_border_width(rect, 0, LV_PART_MAIN);
        lv_obj_set_style_bg_color(rect, lv_color_make(shade, shade, shade), LV_PART_MAIN);
    }

    for (int i = 0; i < 8; i++) {
        char txt[4];
        sprintf(txt, "%d", i);
        lv_obj_t *label = lv_label_create(screen);
        lv_label_set_text(label, txt);
        lv_obj_set_style_text_color(label, lv_color_black(), 0);
        lv_obj_align(label, LV_ALIGN_TOP_LEFT, i * colW + colW / 2 - 5, 10);
    }

    lv_refr_now(lv_display_get_default()); // Render LVGL scene into framebuffer
    display.display();                     // Push framebuffer to e-paper panel
}
