/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       Display JPEG image from SD card on Soldered Inkplate 5.
 *
 * @details     Initializes the SD card, loads cat.jpg from the root directory
 *              using the LVGL SD filesystem driver (registered automatically by
 *              the Inkplate constructor), displays it, then puts the SD card
 *              to sleep for power saving.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 5
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 5, USB cable, microSD card with cat.jpg in root
 * - Extra:      microSD card
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate5
 *
 * How to use:
 * 1) Copy cat.jpg to the root of a FAT-formatted microSD card.
 * 2) Insert the SD card into Inkplate 5.
 * 3) Build and flash.
 * 4) The cat image is loaded from the SD card and shown on the display.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 */

#include "sdkconfig.h"

#ifndef CONFIG_INKPLATE_BOARD_INKPLATE5
#error \
    "Wrong board selection for this example, please select Inkplate5 in the boards menu."
#endif

#include "Inkplate.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);
    display.setDisplayMode(BLACK_AND_WHITE);
    display.enableDithering(true);

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    lv_obj_t *statusLabel = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(statusLabel, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_align(statusLabel, LV_ALIGN_TOP_MID, 0, 10);

    if (display.sdCardInit() != ESP_OK) {
        lv_label_set_text(statusLabel, "SD Card init failed!");
        lv_refr_now(lv_display_get_default());
        display.display();
        return;
    }

    // LVGL SD filesystem driver registered automatically by the Inkplate constructor
    lv_obj_t *img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, "S:/cat.jpg");
    lv_obj_center(img);

    lv_label_set_text(statusLabel, "Image loaded from SD");

    lv_refr_now(lv_display_get_default());
    display.display();

    display.sdCardSleep();

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
