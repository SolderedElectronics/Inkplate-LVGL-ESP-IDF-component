/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       Display image from RAM buffer on Soldered Inkplate 4.
 *
 * @details     Renders a 600x600 L8 (grayscale, 1 byte/pixel) image stored in
 *              cat.h using an LVGL image descriptor. Dithering is enabled for
 *              best output on the black-and-white e-paper panel.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 4
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 4, USB cable
 * - Extra:      None
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate4
 *
 * How to use:
 * 1) Replace cat.h with the actual image (see cat.h for instructions).
 * 2) Build and flash to Inkplate 4.
 * 3) The cat image is shown on the display.
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
#include "cat.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// cat.h: 600x600, L8 grayscale (1 byte/pixel), array size = CAT_WIDTH * CAT_HEIGHT
static lv_image_dsc_t catImage;

static void initCatImage(void) {
    catImage = {};
    catImage.header.cf = LV_COLOR_FORMAT_L8;
    catImage.header.w  = CAT_WIDTH;
    catImage.header.h  = CAT_HEIGHT;
    catImage.data_size = CAT_WIDTH * CAT_HEIGHT;
    catImage.data      = cat;
}

extern "C" void app_main(void) {
    initCatImage();

    Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);
    display.setDisplayMode(BLACK_AND_WHITE);
    display.enableDithering(true);

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    lv_obj_t *img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, &catImage);
    lv_obj_center(img);

    lv_refr_now(lv_display_get_default());
    display.display();

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
