/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       Frontlight brightness control example for Inkplate 4.
 *
 * @details     Sweeps the built-in LED frontlight from minimum (0) to maximum
 *              (63) brightness and back in a continuous loop. The current
 *              brightness level is shown on the e-paper display and updated
 *              every 8 steps.
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
 * 2) Watch the frontlight LEDs brighten and dim; the display shows the level.
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
#include "freertos/task.h"
#include <stdio.h>

#define FRONTLIGHT_MAX   63
#define STEP_DELAY_MS    30
#define DISPLAY_EVERY    8

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_PARTIAL);
    display.setDisplayMode(BLACK_AND_WHITE);

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    lv_obj_t *lbl_title = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(lbl_title, lv_color_hex(0x000000), 0);
    lv_label_set_text(lbl_title, "Frontlight Demo");
    lv_obj_align(lbl_title, LV_ALIGN_TOP_MID, 0, 80);

    lv_obj_t *lbl_brightness = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(lbl_brightness, lv_color_hex(0x000000), 0);
    lv_label_set_text(lbl_brightness, "Brightness: 0 / 63");
    lv_obj_align(lbl_brightness, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *lbl_hint = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(lbl_hint, lv_color_hex(0x000000), 0);
    lv_label_set_text(lbl_hint, "Sweeping 0 -> 63 -> 0");
    lv_obj_align(lbl_hint, LV_ALIGN_BOTTOM_MID, 0, -80);

    lv_refr_now(lv_display_get_default());
    display.display();

    display.frontlight.setState(true);

    int b = 0;
    int direction = 1;
    int stepsSinceRefresh = 0;
    char buf[32];

    while (1) {
        display.frontlight.setBrightness((uint8_t)b);

        if (++stepsSinceRefresh >= DISPLAY_EVERY) {
            snprintf(buf, sizeof(buf), "Brightness: %d / 63", b);
            lv_label_set_text(lbl_brightness, buf);
            lv_refr_now(lv_display_get_default());
            display.partialUpdate();
            stepsSinceRefresh = 0;
        }

        b += direction;
        if (b >= FRONTLIGHT_MAX) {
            b = FRONTLIGHT_MAX;
            direction = -1;
        } else if (b <= 0) {
            b = 0;
            direction = 1;
        }

        vTaskDelay(pdMS_TO_TICKS(STEP_DELAY_MS));
    }
}
