/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       Image from buffer example for Soldered Inkplate 6Color with LVGL.
 *
 * @details     Displays a JPG image stored as a C array (cat.h) directly from
 *              flash memory using LVGL's image widget with RGB888 format.
 *              Dithering is enabled for best color reproduction on the 7-color
 *              e-paper panel.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6Color
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 6Color, USB cable
 * - Extra:      cat.h image array (included in this example)
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate6 Color
 *
 * How to use:
 * 1) Build and flash to Inkplate 6Color.
 * 2) Cat image appears centered on the display.
 *
 * Notes:
 * - cat.h was generated with https://notisrac.github.io/FileToCArray/
 *   Options: Hex(0x00), 24bit RGB, Multi line, Separate bytes of pixels.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 */

#define PROGMEM  // AVR-specific; defined empty for ESP-IDF

#include "sdkconfig.h"

#ifndef CONFIG_INKPLATE_BOARD_INKPLATE6COLOR
#error \
    "Wrong board selection for this example, please select Inkplate6Color in the boards menu."
#endif

#include "Inkplate.h"
#include "cat.h"

static const lv_image_dsc_t my_image = {
    .header = {
        .magic      = LV_IMAGE_HEADER_MAGIC,
        .cf         = LV_COLOR_FORMAT_RGB888,
        .flags      = 0,
        .w          = CAT_WIDTH,
        .h          = CAT_HEIGHT,
        .stride     = CAT_WIDTH * 3,
        .reserved_2 = 0,
    },
    .data_size  = CAT_WIDTH * CAT_HEIGHT * 3,
    .data       = cat,
    .reserved   = nullptr,
    .reserved_2 = nullptr,
};

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);
    display.enableDithering(true);

    lv_obj_t *screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    lv_obj_t *img = lv_image_create(screen);
    lv_image_set_src(img, &my_image);
    lv_obj_center(img);

    lv_refr_now(lv_display_get_default());
    display.display();
}
