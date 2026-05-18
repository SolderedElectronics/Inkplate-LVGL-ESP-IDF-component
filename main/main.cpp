#include "Inkplate.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "lvgl.h"
extern const lv_image_dsc_t cat;
#ifdef __cplusplus
}
#endif

static const char *TAG = "MAIN";

extern "C" void app_main(void) {
    Inkplate inkplate(LV_DISPLAY_RENDER_MODE_FULL);
    inkplate.enableDithering(true);

    lv_obj_t *screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    lv_obj_t *img = lv_image_create(screen);
    lv_image_set_src(img, &cat);
    lv_obj_center(img);

    for (int i = 0; i < 5; i++) {
        lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    inkplate.display();
    ESP_LOGI(TAG, "Display updated with RGB888 image on Inkplate 13");
}
