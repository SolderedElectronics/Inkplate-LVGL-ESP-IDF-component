/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       NVS (EEPROM equivalent) usage example for Soldered Inkplate 10.
 *
 * @details     Demonstrates storing and reading persistent data using NVS
 *              (Non-Volatile Storage), which replaces Arduino EEPROM in ESP-IDF.
 *              Writes 128 bytes to the "user_data" NVS namespace, reads them
 *              back, and verifies the data. Result is shown on the display.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 10
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 10, USB cable
 * - Extra:      None
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate10
 *
 * How to use:
 * 1) Build and flash to Inkplate 10.
 * 2) Display shows whether the write/read/verify cycle succeeded.
 *
 * Notes:
 * - NVS "user_data" namespace is separate from Inkplate's internal "inkplate"
 *   namespace used for waveform data — no conflict.
 * - Data persists across reboots until explicitly erased.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 */

#include "sdkconfig.h"

#ifndef CONFIG_INKPLATE_BOARD_INKPLATE10
#error \
    "Wrong board selection for this example, please select Inkplate10 in the boards menu."
#endif

#include "Inkplate.h"
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"

static const char *TAG = "EEPROM_USAGE";

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);
    display.setDisplayMode(BLACK_AND_WHITE);

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    lv_obj_t *label = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(label, lv_color_hex(0x000000), 0);
    lv_label_set_text(label, "Testing NVS storage...");
    lv_obj_center(label);

    lv_refr_now(lv_display_get_default());
    display.display();

    // NVS is already initialized by the Inkplate constructor.
    // Open a separate namespace so we don't interfere with Inkplate's data.
    nvs_handle_t handle;
    esp_err_t err = nvs_open("user_data", NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "nvs_open failed: %s", esp_err_to_name(err));
        lv_label_set_text(label, "NVS open failed!");
        lv_obj_center(label);
        lv_refr_now(lv_display_get_default());
        display.display();
        return;
    }

    // Write 128 bytes — values 0..127
    uint8_t writeData[128];
    for (int i = 0; i < 128; i++) {
        writeData[i] = (uint8_t)i;
    }

    err = nvs_set_blob(handle, "data", writeData, sizeof(writeData));
    if (err == ESP_OK) {
        err = nvs_commit(handle);
    }
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "NVS write failed: %s", esp_err_to_name(err));
        nvs_close(handle);
        lv_label_set_text(label, "NVS write failed!");
        lv_obj_center(label);
        lv_refr_now(lv_display_get_default());
        display.display();
        return;
    }
    ESP_LOGI(TAG, "Wrote 128 bytes to NVS");

    // Read back and verify
    uint8_t readData[128] = {};
    size_t  readLen       = sizeof(readData);
    err = nvs_get_blob(handle, "data", readData, &readLen);
    nvs_close(handle);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "NVS read failed: %s", esp_err_to_name(err));
        lv_label_set_text(label, "NVS read failed!");
        lv_obj_center(label);
        lv_refr_now(lv_display_get_default());
        display.display();
        return;
    }

    bool ok = true;
    for (int i = 0; i < 128; i++) {
        if (readData[i] != (uint8_t)i) {
            ok = false;
            break;
        }
    }

    ESP_LOGI(TAG, "NVS verify: %s", ok ? "PASS" : "FAIL");
    lv_label_set_text(label, ok ? "NVS OK!\nAll 128 bytes verified." : "NVS verify FAILED!");
    lv_obj_center(label);

    lv_refr_now(lv_display_get_default());
    display.display();
}
