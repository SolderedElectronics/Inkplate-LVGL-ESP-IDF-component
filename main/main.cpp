#include "Inkplate.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "lvgl.h"
#include <stdio.h>

static const char *TAG = "MAIN";

extern "C" void app_main(void) {
    Inkplate inkplate(LV_DISPLAY_RENDER_MODE_FULL);
    inkplate.enableDithering(true);

    if (inkplate.sdCardInit() != ESP_OK) {
        ESP_LOGE(TAG, "SD card init failed");
        return;
    }

    // Read JPEG into PSRAM
    FILE *f = fopen("/sdcard/cat.jpg", "rb");
    if (!f) {
        ESP_LOGE(TAG, "Failed to open /sdcard/cat.jpg");
        return;
    }
    fseek(f, 0, SEEK_END);
    size_t file_size = ftell(f);
    fseek(f, 0, SEEK_SET);

    uint8_t *jpeg_buf = (uint8_t *)heap_caps_malloc(file_size, MALLOC_CAP_SPIRAM);
    if (!jpeg_buf) {
        ESP_LOGE(TAG, "Failed to allocate %u bytes for JPEG", file_size);
        fclose(f);
        return;
    }
    fread(jpeg_buf, 1, file_size, f);
    fclose(f);
    // inkplate.sdCardSleep();
    ESP_LOGI(TAG, "JPEG read: %u bytes", file_size);

    // LV_COLOR_FORMAT_RAW + JPEG magic bytes → tjpgd decodes it automatically
    static lv_image_dsc_t img_dsc;
    img_dsc.header.magic  = LV_IMAGE_HEADER_MAGIC;
    img_dsc.header.cf     = LV_COLOR_FORMAT_RAW;
    img_dsc.header.w      = 1024;
    img_dsc.header.h      = 758;
    img_dsc.header.stride = 0;
    img_dsc.data_size     = file_size;
    img_dsc.data          = jpeg_buf;

    lv_obj_t *img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, &img_dsc);
    lv_obj_center(img);

    ESP_LOGI(TAG, "Starting render...");
    lv_refr_now(inkplate.getDisplay());
    ESP_LOGI(TAG, "Render done, sending to display...");
    inkplate.display();
    ESP_LOGI(TAG, "Display update triggered");

    heap_caps_free(jpeg_buf);
}
