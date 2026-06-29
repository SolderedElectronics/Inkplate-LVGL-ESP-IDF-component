/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       Simple touchscreen example for Soldered Inkplate 6Flick with LVGL.
 *
 * @details     Demonstrates direct touchscreen polling on the Inkplate 6Flick.
 *              A rectangle is drawn on screen; each tap moves it diagonally until
 *              it reaches the bottom, then it resets to the top-left.
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
 * 2) Tap the black rectangle to move it diagonally across the screen.
 *
 * Notes:
 * - The touchscreen is initialized automatically by the Inkplate constructor.
 * - Touch events are detected via a FreeRTOS semaphore given from the GPIO ISR.
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
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#define RECT_W 100
#define RECT_H 50

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_PARTIAL);
    display.setDisplayMode(BLACK_AND_WHITE);
    display.frontlight.setState(false);

    lv_obj_t *label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Touch the rectangle on screen!");
    lv_obj_set_style_text_color(label, lv_color_hex(0x000000), 0);
    lv_obj_center(label);

    lv_refr_now(lv_display_get_default());
    display.display();

    vTaskDelay(pdMS_TO_TICKS(2000));

    lv_obj_del(label);
    display.clearDisplay();

    int x_pos = 50;
    int y_pos = 50;

    lv_obj_t *rect = lv_obj_create(lv_screen_active());
    lv_obj_set_style_bg_color(rect, lv_color_hex(0x000000), 0);
    lv_obj_set_style_border_width(rect, 0, LV_PART_MAIN);
    lv_obj_set_size(rect, RECT_W, RECT_H);
    lv_obj_set_pos(rect, x_pos, y_pos);

    lv_refr_now(lv_display_get_default());
    display.display();

    SemaphoreHandle_t sem = display.touchscreen.getTouchSemaphore();

    while (1) {
        // Block until touch interrupt fires (press or release).
        if (xSemaphoreTake(sem, pdMS_TO_TICKS(100)) == pdTRUE) {
            uint16_t x[2], y[2];
            uint8_t n = display.touchscreen.getData(x, y);
            // Drain the paired release event immediately so it doesn't
            // consume the next semaphore take as a spurious n=0.
            xSemaphoreTake(sem, 0);

            printf("n=%d x=%d y=%d | rect x=[%d,%d] y=[%d,%d]\n",
                   n, x[0], y[0], x_pos, x_pos + RECT_W, y_pos, y_pos + RECT_H);

            if (n >= 1 &&
                x[0] >= (uint16_t)x_pos && x[0] <= (uint16_t)(x_pos + RECT_W) &&
                y[0] >= (uint16_t)y_pos && y[0] <= (uint16_t)(y_pos + RECT_H)) {

                x_pos += 100;
                y_pos += 100;
                if (y_pos >= 660) {
                    x_pos = 50;
                    y_pos = 50;
                }
                lv_obj_set_pos(rect, x_pos, y_pos);
                lv_refr_now(lv_display_get_default());
                display.partialUpdate();
            }
        }
    }
}
