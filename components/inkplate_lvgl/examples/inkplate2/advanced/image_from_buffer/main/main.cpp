/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       Image from buffer example for Soldered Inkplate 2 with LVGL.
 *
 * @details     Demonstrates how to display an RGB565 image stored in a C array
 *              on the Inkplate 2 e-paper display using LVGL's image widget.
 *              Dithering is enabled to improve the visual quality of the image
 *              on the 3-color (black, white, red) e-paper panel.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 2
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 2, USB cable
 * - Extra:      None
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate2
 *
 * How to use:
 * 1) Build and flash to Inkplate 2.
 * 2) The image stored in image_ex.h is rendered to the display.
 *
 * Expected output:
 * - Display shows the image from image_ex.h centered on screen.
 *
 * Notes:
 * - image_ex.h contains RGB565 pixel data at 212x104 resolution.
 * - To use a custom image, replace image_ex.h with your own converted array.
 * - Use https://tools.soldered.com/tools/image-converter/ to generate arrays.
 * - Dithering is only supported in FULL render mode.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 */

#include "sdkconfig.h"

#ifndef CONFIG_INKPLATE_BOARD_INKPLATE2
#error                                                                         \
    "Wrong board selection for this example, please select Inkplate2 in the boards menu."
#endif

#include "Inkplate.h"
#include "image_ex.h"

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);
    display.enableDithering(true);

    lv_obj_t *screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    lv_obj_t *img = lv_image_create(screen);
    lv_image_set_src(img, &image_ex);
    lv_obj_center(img);

    lv_refr_now(lv_display_get_default());
    display.display();
}
