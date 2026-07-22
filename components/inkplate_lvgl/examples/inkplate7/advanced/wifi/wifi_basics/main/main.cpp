/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       WiFi basics example for Soldered Inkplate 7 with LVGL.
 *
 * @details     Connects to a WiFi network using credentials set in menuconfig
 *              and displays the connection status on the e-paper screen.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 7
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 7, USB cable
 * - Extra:      Stable WiFi connection
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate7
 * - Menuconfig -> WiFi Configuration -> Enter your SSID and password
 *
 * How to use:
 * 1) Set WiFi credentials via menuconfig.
 * 2) Build and flash to Inkplate 7.
 * 3) "Connected!" appears on screen when WiFi is established.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 */

#include "sdkconfig.h"

#ifndef CONFIG_INKPLATE_BOARD_INKPLATE7
#error                                                                         \
    "Wrong board selection for this example, please select Inkplate7 in the boards menu."
#endif

#include "Inkplate.h"
#include "WiFi.h"

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);
    display.enableDithering(true);

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    lv_obj_t *label = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(label, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_28, 0);
    lv_label_set_text(label, "Connecting to WiFi...");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    lv_refr_now(lv_display_get_default());
    display.display();

    WiFi wifi;
    if (wifi.begin() != ESP_OK || !wifi.waitForConnect(10000)) {
        lv_label_set_text(label, "WiFi connection failed!");
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
        lv_refr_now(lv_display_get_default());
        display.display();
        return;
    }

    lv_label_set_text(label, "Connected!");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    lv_refr_now(lv_display_get_default());
    display.display();
}
