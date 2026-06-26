/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       BQ27441 battery fuel gauge example for Inkplate 4.
 *
 * @details     Reads voltage, state of charge, average current, and remaining
 *              capacity from the built-in BQ27441 LiPo fuel gauge IC. Display
 *              updates via partial refresh every 10 seconds; full refresh every
 *              10 updates.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 4
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 4, USB cable, LiPo battery connected
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate4
 *
 * How to use:
 * 1) Connect a LiPo battery to Inkplate 4.
 * 2) Build and flash.
 * 3) Display shows battery voltage, SoC, current, and remaining capacity.
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

#define FULL_REFRESH_EVERY 10

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_PARTIAL);
    display.setDisplayMode(BLACK_AND_WHITE);

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    lv_obj_t *lbl_title = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(lbl_title, lv_color_hex(0x000000), 0);
    lv_label_set_text(lbl_title, "BQ27441 Fuel Gauge");
    lv_obj_align(lbl_title, LV_ALIGN_TOP_MID, 0, 20);

    lv_obj_t *lbl_voltage = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(lbl_voltage, lv_color_hex(0x000000), 0);
    lv_label_set_text(lbl_voltage, "Voltage: ---- mV");
    lv_obj_set_pos(lbl_voltage, 20, 110);

    lv_obj_t *lbl_soc = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(lbl_soc, lv_color_hex(0x000000), 0);
    lv_label_set_text(lbl_soc, "State of Charge: -- %");
    lv_obj_set_pos(lbl_soc, 20, 230);

    lv_obj_t *lbl_current = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(lbl_current, lv_color_hex(0x000000), 0);
    lv_label_set_text(lbl_current, "Current: ---- mA");
    lv_obj_set_pos(lbl_current, 20, 350);

    lv_obj_t *lbl_cap = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(lbl_cap, lv_color_hex(0x000000), 0);
    lv_label_set_text(lbl_cap, "Remaining: ---- mAh");
    lv_obj_set_pos(lbl_cap, 20, 470);

    lv_refr_now(lv_display_get_default());
    display.display();

    int updateCount = 0;
    char buf[64];

    while (1) {
        uint16_t voltage = display.bq.voltage();
        uint16_t soc     = display.bq.soc();
        int16_t  current = display.bq.current();
        uint16_t cap     = display.bq.capacity();

        snprintf(buf, sizeof(buf), "Voltage: %u mV", voltage);
        lv_label_set_text(lbl_voltage, buf);

        snprintf(buf, sizeof(buf), "State of Charge: %u %%", soc);
        lv_label_set_text(lbl_soc, buf);

        snprintf(buf, sizeof(buf), "Current: %d mA", (int)current);
        lv_label_set_text(lbl_current, buf);

        snprintf(buf, sizeof(buf), "Remaining: %u mAh", cap);
        lv_label_set_text(lbl_cap, buf);

        lv_refr_now(lv_display_get_default());
        if (++updateCount >= FULL_REFRESH_EVERY) {
            display.display();
            updateCount = 0;
        } else {
            display.partialUpdate();
        }

        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
