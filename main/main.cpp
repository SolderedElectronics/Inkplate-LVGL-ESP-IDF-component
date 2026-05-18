#include "Inkplate.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "MAIN";

static lv_obj_t *make_rect(lv_obj_t *parent, int x, int y, int w, int h, lv_color_t color) {
    lv_obj_t *obj = lv_obj_create(parent);
    lv_obj_remove_style_all(obj);
    lv_obj_set_pos(obj, x, y);
    lv_obj_set_size(obj, w, h);
    lv_obj_set_style_bg_color(obj, color, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    return obj;
}

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);
    display.enableDithering(true);

    lv_obj_t *screen = lv_scr_act();
    lv_obj_remove_style_all(screen);
    lv_obj_set_style_bg_color(screen, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    // Full refresh: white bg + black rectangle
    make_rect(screen, 100, 100, 600, 400, lv_color_black());

    lv_refr_now(lv_display_get_default());
    display.display();
    ESP_LOGI(TAG, "Full update done");

    vTaskDelay(pdMS_TO_TICKS(2000));

    // Partial update: red box + label inside the black rect
    lv_obj_t *box = make_rect(screen, 150, 150, 300, 200, lv_color_make(255, 0, 0));

    lv_obj_t *label = lv_label_create(box);
    lv_obj_remove_style_all(label);
    lv_label_set_text(label, "Partial!");
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_48, 0);
    lv_obj_center(label);

    lv_refr_now(lv_display_get_default());
    display.displayPartial(150, 150, 300, 200);
    ESP_LOGI(TAG, "Partial update done");
}
