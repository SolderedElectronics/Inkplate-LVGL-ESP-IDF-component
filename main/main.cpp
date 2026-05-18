#include "Inkplate.h"
#include "esp_log.h"
#include <cstring>

#define IMAGE_URL "https://raw.githubusercontent.com/SolderedElectronics/Inkplate-LVGL-Library/refs/heads/dev/examples/Inkplate10/Advanced/SD/ImageFromSD/cat.jpg"

static const char *TAG = "MAIN";

extern "C" void app_main(void)
{
    Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);
    display.enableDithering(true);

    display.wifi.begin();
    if (!display.wifi.waitForConnect(15000)) {
        ESP_LOGE(TAG, "WiFi connection timed out");
        return;
    }
    ESP_LOGI(TAG, "WiFi connected");

    display.wifi.setCurrentTime();

    int32_t len = 0;
    uint8_t *imgData = display.wifi.downloadFileHTTPS(IMAGE_URL, &len);
    if (!imgData || len <= 0) {
        ESP_LOGE(TAG, "Image download failed");
        return;
    }
    ESP_LOGI(TAG, "Downloaded %ld bytes", (long)len);

    lv_fs_path_ex_t path;
    lv_fs_make_path_from_buffer(&path, 'M', imgData, (uint32_t)len, "jpg");

    lv_obj_t *screen = lv_scr_act();
    lv_obj_remove_style_all(screen);
    lv_obj_set_style_bg_color(screen, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    lv_obj_t *img = lv_image_create(screen);
    lv_image_set_src(img, (const char *)&path);
    lv_obj_center(img);

    lv_refr_now(lv_display_get_default());
    display.display();
    ESP_LOGI(TAG, "Display updated");

    free(imgData);
}
