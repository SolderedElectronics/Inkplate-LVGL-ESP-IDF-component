#include "Inkplate.h"
#include "esp_log.h"

static const char *TAG = "MAIN";

extern "C" void app_main(void) {
  Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);
  display.enableDithering(true);

  if (display.sdCardInit() != ESP_OK) {
    ESP_LOGE(TAG, "SD card init failed");
    return;
  }
  ESP_LOGI(TAG, "SD card initialized");

  lv_obj_t *screen = lv_scr_act();
  lv_obj_set_style_bg_color(screen, lv_color_white(), 0);
  lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

  lv_obj_t *label = lv_label_create(screen);
  lv_label_set_text(label, "Image loaded from SD");
  lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 10);

  lv_obj_t *img = lv_image_create(screen);
  lv_image_set_src(img, "S:/cat.jpg");
  lv_obj_center(img);

  lv_refr_now(lv_display_get_default());
  display.display();
  ESP_LOGI(TAG, "Display updated");

  display.sdCardSleep();
}
