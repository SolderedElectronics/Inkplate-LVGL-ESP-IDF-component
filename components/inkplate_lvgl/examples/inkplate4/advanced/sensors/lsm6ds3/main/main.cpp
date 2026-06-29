/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       LSM6DS3 accelerometer and gyroscope example for Inkplate 4.
 *
 * @details     Reads X/Y/Z acceleration (in g) and X/Y/Z angular rate (deg/s)
 *              from the built-in LSM6DS3 IMU. Display updates via partial
 *              refresh; full refresh every 10 updates.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 4
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 4, USB cable
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate4
 *
 * How to use:
 * 1) Build and flash to Inkplate 4.
 * 2) Tilt the board to change accelerometer readings.
 * 3) Display shows live accelerometer and gyroscope values.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 */

#include "sdkconfig.h"

#ifndef CONFIG_INKPLATE_BOARD_INKPLATE4
#error \
    "Wrong board selection for this example, please select Inkplate4 in the boards menu."
#endif

#include "Inkplate.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

#define FULL_REFRESH_EVERY 10

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_PARTIAL);
    display.setDisplayMode(BLACK_AND_WHITE);

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    lv_obj_t *lbl_title = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(lbl_title, lv_color_hex(0x000000), 0);
    lv_label_set_text(lbl_title, "LSM6DS3 IMU");
    lv_obj_align(lbl_title, LV_ALIGN_TOP_MID, 0, 20);

    lv_obj_t *lbl_accel_header = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(lbl_accel_header, lv_color_hex(0x000000), 0);
    lv_label_set_text(lbl_accel_header, "-- Accelerometer (g) --");
    lv_obj_set_pos(lbl_accel_header, 20, 80);

    lv_obj_t *lbl_ax = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(lbl_ax, lv_color_hex(0x000000), 0);
    lv_label_set_text(lbl_ax, "X: 0.0000");
    lv_obj_set_pos(lbl_ax, 20, 130);

    lv_obj_t *lbl_ay = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(lbl_ay, lv_color_hex(0x000000), 0);
    lv_label_set_text(lbl_ay, "Y: 0.0000");
    lv_obj_set_pos(lbl_ay, 20, 185);

    lv_obj_t *lbl_az = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(lbl_az, lv_color_hex(0x000000), 0);
    lv_label_set_text(lbl_az, "Z: 0.0000");
    lv_obj_set_pos(lbl_az, 20, 240);

    lv_obj_t *lbl_gyro_header = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(lbl_gyro_header, lv_color_hex(0x000000), 0);
    lv_label_set_text(lbl_gyro_header, "-- Gyroscope (deg/s) --");
    lv_obj_set_pos(lbl_gyro_header, 20, 310);

    lv_obj_t *lbl_gx = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(lbl_gx, lv_color_hex(0x000000), 0);
    lv_label_set_text(lbl_gx, "X: 0.0000");
    lv_obj_set_pos(lbl_gx, 20, 360);

    lv_obj_t *lbl_gy = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(lbl_gy, lv_color_hex(0x000000), 0);
    lv_label_set_text(lbl_gy, "Y: 0.0000");
    lv_obj_set_pos(lbl_gy, 20, 415);

    lv_obj_t *lbl_gz = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(lbl_gz, lv_color_hex(0x000000), 0);
    lv_label_set_text(lbl_gz, "Z: 0.0000");
    lv_obj_set_pos(lbl_gz, 20, 470);

    lv_refr_now(lv_display_get_default());
    display.display();

    int updateCount = 0;
    char buf[48];

    while (1) {
        float ax = display.lsm.readFloatAccelX();
        float ay = display.lsm.readFloatAccelY();
        float az = display.lsm.readFloatAccelZ();
        float gx = display.lsm.readFloatGyroX();
        float gy = display.lsm.readFloatGyroY();
        float gz = display.lsm.readFloatGyroZ();

        snprintf(buf, sizeof(buf), "X: %.4f", ax);
        lv_label_set_text(lbl_ax, buf);

        snprintf(buf, sizeof(buf), "Y: %.4f", ay);
        lv_label_set_text(lbl_ay, buf);

        snprintf(buf, sizeof(buf), "Z: %.4f", az);
        lv_label_set_text(lbl_az, buf);

        snprintf(buf, sizeof(buf), "X: %.4f", gx);
        lv_label_set_text(lbl_gx, buf);

        snprintf(buf, sizeof(buf), "Y: %.4f", gy);
        lv_label_set_text(lbl_gy, buf);

        snprintf(buf, sizeof(buf), "Z: %.4f", gz);
        lv_label_set_text(lbl_gz, buf);

        lv_refr_now(lv_display_get_default());
        if (++updateCount >= FULL_REFRESH_EVERY) {
            display.display();
            updateCount = 0;
        } else {
            display.partialUpdate();
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
