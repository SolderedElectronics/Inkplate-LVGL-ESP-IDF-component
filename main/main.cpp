#include "Inkplate2.h"
#include "esp_heap_caps.h"
#include "lvgl.h"

extern "C" void app_main(void) {
    lv_init();

    static Inkplate2 inkplate;

    const size_t buf_size = E_INK_WIDTH * E_INK_HEIGHT * sizeof(lv_color_t);
    lv_color_t *buf =
        (lv_color_t *)heap_caps_malloc(buf_size, MALLOC_CAP_SPIRAM);

    lv_display_t *disp = lv_display_create(E_INK_HEIGHT, E_INK_WIDTH);
    lv_display_set_buffers(disp, buf, NULL, buf_size,
                           LV_DISPLAY_RENDER_MODE_FULL);
    lv_display_set_flush_cb(disp, display_flush_callback);
    lv_display_set_user_data(disp, &inkplate);

    lv_obj_t *scr = lv_display_get_screen_active(disp);
    lv_obj_set_style_bg_color(scr, lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    const int NUM_RECTS = 3;
    const int RECT_WIDTH = E_INK_HEIGHT / NUM_RECTS;

    uint32_t colors[NUM_RECTS] = {
        0x000000, // Black
        0xFFFFFF, // White
        0xFF0000, // Red
    };

    for (int i = 0; i < NUM_RECTS; i++) {
        lv_obj_t *rect = lv_obj_create(scr);
        lv_obj_set_size(rect, RECT_WIDTH, E_INK_WIDTH);
        lv_obj_set_pos(rect, i * RECT_WIDTH, 0);
        lv_obj_set_style_bg_color(rect, lv_color_hex(colors[i]), LV_PART_MAIN);
        lv_obj_set_style_border_width(rect, 0, LV_PART_MAIN);
        lv_obj_set_style_radius(rect, 0, LV_PART_MAIN);
    }

    lv_refr_now(disp);
    inkplate.display();
}
