/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       BME680 environmental sensor example for Inkplate 4.
 *
 * @details     Reads temperature, humidity, pressure, gas resistance, and
 *              calculated altitude from the built-in BME680 sensor. Display
 *              updates via partial refresh every second; full refresh every
 *              10 updates.
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
 * 2) Display shows live environmental readings updating every second.
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
    lv_label_set_text(lbl_title, "BME680 Environmental Sensor");
    lv_obj_align(lbl_title, LV_ALIGN_TOP_MID, 0, 20);

    lv_obj_t *lbl_temp = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(lbl_temp, lv_color_hex(0x000000), 0);
    lv_label_set_text(lbl_temp, "Temperature: --.-- C");
    lv_obj_set_pos(lbl_temp, 20, 90);

    lv_obj_t *lbl_hum = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(lbl_hum, lv_color_hex(0x000000), 0);
    lv_label_set_text(lbl_hum, "Humidity: --.-- %");
    lv_obj_set_pos(lbl_hum, 20, 200);

    lv_obj_t *lbl_press = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(lbl_press, lv_color_hex(0x000000), 0);
    lv_label_set_text(lbl_press, "Pressure: ----.-- hPa");
    lv_obj_set_pos(lbl_press, 20, 310);

    lv_obj_t *lbl_gas = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(lbl_gas, lv_color_hex(0x000000), 0);
    lv_label_set_text(lbl_gas, "Gas: ----.-- mOhm");
    lv_obj_set_pos(lbl_gas, 20, 420);

    lv_obj_t *lbl_alt = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(lbl_alt, lv_color_hex(0x000000), 0);
    lv_label_set_text(lbl_alt, "Altitude: ---.-- m");
    lv_obj_set_pos(lbl_alt, 20, 520);

    lv_refr_now(lv_display_get_default());
    display.display();

    int updateCount = 0;
    char buf[64];

    while (1) {
        float temp, hum, press, gas;
        display.bme.readSensorData(temp, hum, press, gas);
        float alt = display.bme.calculateAltitude(press);

        snprintf(buf, sizeof(buf), "Temperature: %.2f C", temp);
        lv_label_set_text(lbl_temp, buf);

        snprintf(buf, sizeof(buf), "Humidity: %.2f %%", hum);
        lv_label_set_text(lbl_hum, buf);

        snprintf(buf, sizeof(buf), "Pressure: %.2f hPa", press);
        lv_label_set_text(lbl_press, buf);

        snprintf(buf, sizeof(buf), "Gas: %.2f mOhm", gas);
        lv_label_set_text(lbl_gas, buf);

        snprintf(buf, sizeof(buf), "Altitude: %.2f m", alt);
        lv_label_set_text(lbl_alt, buf);

        lv_refr_now(lv_display_get_default());
        if (++updateCount >= FULL_REFRESH_EVERY) {
            display.display();
            updateCount = 0;
        } else {
            display.partialUpdate();
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
