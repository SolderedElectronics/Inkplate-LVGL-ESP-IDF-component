/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       Read battery voltage example for Soldered Inkplate 6Color with LVGL.
 *
 * @details     Reads the LiPo battery voltage via the onboard ADC and displays
 *              it on screen. The display refreshes every 60 seconds.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6Color
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 6Color, USB cable, LiPo battery (optional)
 * - Extra:      None
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate6 Color
 *
 * How to use:
 * 1) Build and flash to Inkplate 6Color.
 * 2) Battery voltage updates on screen every 60 seconds.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 */

#include "sdkconfig.h"

#ifndef CONFIG_INKPLATE_BOARD_INKPLATE6COLOR
#error \
    "Wrong board selection for this example, please select Inkplate6Color in the boards menu."
#endif

#include "Inkplate.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    lv_obj_t *label = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_font(label, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0x0000AA), LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    while (1) {
        float voltage = display.readBattery();

        char txt[32];
        snprintf(txt, sizeof(txt), "%.2f V", voltage);

        lv_label_set_text(label, txt);
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

        lv_refr_now(lv_display_get_default());
        display.display();

        vTaskDelay(pdMS_TO_TICKS(60000));
    }
}
