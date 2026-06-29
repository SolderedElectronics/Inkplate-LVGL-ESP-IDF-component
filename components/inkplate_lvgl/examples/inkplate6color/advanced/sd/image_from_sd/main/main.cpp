/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       Image from SD card example for Soldered Inkplate 6Color with LVGL.
 *
 * @details     Initializes the SD card, then uses LVGL's image widget to decode
 *              and display a JPEG file (cat.jpg) stored on the SD card.
 *              Dithering is enabled for best color reproduction. The SD card is
 *              put to sleep after use to save power.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6Color
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 6Color, USB cable, microSD card
 * - Extra:      cat.jpg placed in the root of the microSD card
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate6 Color
 *
 * How to use:
 * 1) Copy cat.jpg to the root of a FAT32-formatted microSD card.
 * 2) Insert the card, build and flash.
 * 3) Cat image appears centered on the display.
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

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);
    display.enableDithering(true);

    lv_obj_t *screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    if (display.sdCardInit() != ESP_OK) {
        lv_obj_t *label = lv_label_create(screen);
        lv_label_set_text(label, "SD Card init failed!");
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
        lv_refr_now(lv_display_get_default());
        display.display();
        return;
    }

    lv_obj_t *img = lv_image_create(screen);
    lv_image_set_src(img, "S:/cat.jpg");
    lv_obj_center(img);

    lv_obj_t *label = lv_label_create(screen);
    lv_label_set_text(label, "Image loaded from SD");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 10);

    lv_refr_now(lv_display_get_default());
    display.display();

    display.sdCardSleep();
}
