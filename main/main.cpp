#include "Inkplate2.h"
#include "esp_heap_caps.h"
#include "lvgl.h"

extern "C" void app_main(void) {
    lv_init();

    static Inkplate2 inkplate;

    // Full-screen draw buffer in PSRAM (RGB565: 2 bytes per pixel)
    const size_t buf_size = E_INK_WIDTH * E_INK_HEIGHT * sizeof(lv_color_t);
    lv_color_t *buf =
        (lv_color_t *)heap_caps_malloc(buf_size, MALLOC_CAP_SPIRAM);

    lv_display_t *disp = lv_display_create(E_INK_WIDTH, E_INK_HEIGHT);
    lv_display_set_buffers(disp, buf, NULL, buf_size,
                           LV_DISPLAY_RENDER_MODE_FULL);
    lv_display_set_flush_cb(disp, display_flush_callback);
    lv_display_set_user_data(disp, &inkplate);

    lv_obj_t *scr = lv_display_get_screen_active(disp);

    lv_obj_t *label = lv_label_create(scr);
    lv_label_set_text(label, "Hello Inkplate!");
    lv_obj_center(label);

    lv_refr_now(disp);
    inkplate.display();
}