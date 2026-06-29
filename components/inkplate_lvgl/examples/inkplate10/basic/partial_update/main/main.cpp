/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       Partial update moving square example for Soldered Inkplate 10 with LVGL.
 *
 * @details     Demonstrates partial update mode by moving a square across the screen.
 *              Only the changed regions are refreshed, making updates faster than
 *              a full display refresh.
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
 * 2) A square moves diagonally across the screen using partial updates.
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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_PARTIAL);
    display.setDisplayMode(BLACK_AND_WHITE);

    lv_obj_t *screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    lv_obj_t *square = lv_obj_create(screen);
    lv_obj_set_size(square, 50, 50);
    lv_obj_set_style_bg_opa(square, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(square, 4, 0);
    lv_obj_set_style_border_color(square, lv_color_black(), 0);
    lv_obj_set_pos(square, 0, 0);

    lv_refr_now(lv_display_get_default());

    int step = 0;
    while (1) {
        int newX = (step * 20) % (1200 - 50);
        int newY = (step * 20) % (825 - 50);
        lv_obj_set_pos(square, newX, newY);
        step++;

        lv_refr_now(lv_display_get_default());
        display.partialUpdate(0, 1);

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
