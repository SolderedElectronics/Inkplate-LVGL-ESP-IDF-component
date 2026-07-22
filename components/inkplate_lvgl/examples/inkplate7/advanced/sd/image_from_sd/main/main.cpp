/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       Image from SD card example for Soldered Inkplate 7 with LVGL.
 *
 * @details     Initializes the SD card, then uses LVGL's image widget to decode
 *              and display JPEG files stored on the SD card. Displays picture1.jpg
 *              for 5 seconds then switches to picture2.jpg.
 *              Copy picture1.jpg and picture2.jpg to the root of a FAT32-formatted
 *              SD card before running.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 7
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 7, USB cable, microSD card
 * - Extra:      picture1.jpg, picture2.jpg placed in the root of the microSD card
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate7
 *
 * How to use:
 * 1) Copy picture1.jpg and picture2.jpg to the root of a FAT32-formatted SD card.
 * 2) Insert the card, build and flash.
 * 3) picture1.jpg is shown for 5 s, then picture2.jpg.
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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static void showImage(Inkplate &display, const char *path) {
    lv_obj_t *screen = lv_scr_act();
    lv_obj_clean(screen);
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    lv_obj_t *img = lv_image_create(screen);
    lv_image_set_src(img, path);
    lv_obj_center(img);

    lv_refr_now(lv_display_get_default());
    display.display();
}

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);
    display.enableDithering(true);

    lv_obj_t *screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    if (display.sdCardInit() != ESP_OK) {
        lv_obj_t *label = lv_label_create(screen);
        lv_label_set_text(label, "SD Card init failed!");
        lv_obj_center(label);
        lv_refr_now(lv_display_get_default());
        display.display();
        return;
    }

    showImage(display, "S:/picture1.jpg");
    vTaskDelay(pdMS_TO_TICKS(5000));

    showImage(display, "S:/picture2.jpg");
    vTaskDelay(pdMS_TO_TICKS(5000));

    display.sdCardSleep();
}
