/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       Touchscreen example for Inkplate 4.
 *
 * @details     Polls the built-in Elan capacitive touchscreen. When a touch is
 *              detected the coordinates are shown and a filled rectangle moves
 *              to the touch position. Partial refresh is used for fast updates.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 4
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 4, USB cable
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate4
 *
 * How to use:
 * 1) Build and flash to Inkplate 4.
 * 2) Touch the screen — a black square follows your finger.
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
#include "freertos/semphr.h"
#include "freertos/task.h"
#include <stdio.h>

#define SQUARE_SIZE      60
#define FULL_REFRESH_EVERY 30

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_PARTIAL);
    display.setDisplayMode(BLACK_AND_WHITE);

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    lv_obj_t *lbl_hint = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(lbl_hint, lv_color_hex(0x000000), 0);
    lv_label_set_text(lbl_hint, "Touch the screen!");
    lv_obj_align(lbl_hint, LV_ALIGN_TOP_MID, 0, 20);

    lv_obj_t *lbl_coords = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(lbl_coords, lv_color_hex(0x000000), 0);
    lv_label_set_text(lbl_coords, "X: ---  Y: ---");
    lv_obj_align(lbl_coords, LV_ALIGN_TOP_MID, 0, 70);

    // Black square that moves to touch position
    lv_obj_t *square = lv_obj_create(lv_screen_active());
    lv_obj_set_style_bg_color(square, lv_color_hex(0x000000), 0);
    lv_obj_set_style_border_width(square, 0, 0);
    lv_obj_set_size(square, SQUARE_SIZE, SQUARE_SIZE);
    lv_obj_set_pos(square, (600 - SQUARE_SIZE) / 2, (600 - SQUARE_SIZE) / 2);

    lv_refr_now(lv_display_get_default());
    display.display();

    int updateCount = 0;
    char buf[32];

    SemaphoreHandle_t sem = display.touchscreen.getTouchSemaphore();

    while (1) {
        // Block until touch interrupt fires; timeout keeps WDT fed via IDLE.
        if (xSemaphoreTake(sem, pdMS_TO_TICKS(100)) == pdTRUE) {
            uint16_t xPos[2] = {0, 0};
            uint16_t yPos[2] = {0, 0};
            uint8_t n = display.touchscreen.getData(xPos, yPos);
            // Drain paired release event so it doesn't count as a spurious press.
            xSemaphoreTake(sem, 0);

            if (n > 0) {
                // Clamp so square stays on screen
                int sx = (int)xPos[0] - SQUARE_SIZE / 2;
                int sy = (int)yPos[0] - SQUARE_SIZE / 2;
                if (sx < 0) sx = 0;
                if (sy < 0) sy = 0;
                if (sx > 600 - SQUARE_SIZE) sx = 600 - SQUARE_SIZE;
                if (sy > 600 - SQUARE_SIZE) sy = 600 - SQUARE_SIZE;

                lv_obj_set_pos(square, sx, sy);

                snprintf(buf, sizeof(buf), "X: %u  Y: %u", xPos[0], yPos[0]);
                lv_label_set_text(lbl_coords, buf);

                lv_refr_now(lv_display_get_default());
                if (++updateCount >= FULL_REFRESH_EVERY) {
                    display.display();
                    updateCount = 0;
                } else {
                    display.partialUpdate();
                }
            }
        }
    }
}
