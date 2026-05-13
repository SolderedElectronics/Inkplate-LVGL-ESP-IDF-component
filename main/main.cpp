#include "Inkplate.h"
#include "cat.h"
#include "lvgl.h"

extern "C" void app_main(void) {
    Inkplate inkplate;
    inkplate.enableDithering(true);

    static lv_image_dsc_t cat_dsc;
    cat_dsc.header.magic = LV_IMAGE_HEADER_MAGIC;
    cat_dsc.header.cf = LV_COLOR_FORMAT_L8;
    cat_dsc.header.w = CAT_WIDTH;
    cat_dsc.header.h = CAT_HEIGHT;
    cat_dsc.header.stride = CAT_WIDTH;  // L8: 1 byte per pixel
    cat_dsc.data_size = sizeof(cat);
    cat_dsc.data = cat;

    lv_obj_t *img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, &cat_dsc);
    lv_obj_center(img);

    lv_refr_now(inkplate.getDisplay());
    inkplate.display();
}
