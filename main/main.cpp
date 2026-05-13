#include "Inkplate.h"
#include "cat.h"
#include "lvgl.h"

extern "C" void app_main(void) {
    Inkplate inkplate(LV_DISPLAY_RENDER_MODE_FULL);
    inkplate.enableDithering(true);

    static lv_image_dsc_t cat_dsc;
    cat_dsc.header.magic = LV_IMAGE_HEADER_MAGIC;
    cat_dsc.header.cf = LV_COLOR_FORMAT_RGB888;
    cat_dsc.header.w = CAT_WIDTH;
    cat_dsc.header.h = CAT_HEIGHT;
    cat_dsc.header.stride = CAT_WIDTH * 3;
    cat_dsc.data_size = CAT_WIDTH * CAT_HEIGHT * 3;
    cat_dsc.data = cat;

    lv_obj_t *img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, &cat_dsc);
    lv_obj_center(img);

    lv_refr_now(inkplate.getDisplay());
    inkplate.display();
}
