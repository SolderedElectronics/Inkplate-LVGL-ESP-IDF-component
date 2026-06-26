/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       APDS9960 proximity, gesture, and light sensor example for Inkplate 4.
 *
 * @details     Reads proximity, gesture direction, ambient light, and RGB color
 *              from the built-in APDS9960 sensor. Display updates via partial
 *              refresh; full refresh every 30 updates.
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
 * 2) Wave hand over sensor to trigger gesture; bring hand close for proximity.
 * 3) Display shows proximity, last gesture, ambient light, and RGB values.
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

#define FULL_REFRESH_EVERY 30

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_PARTIAL);
    display.setDisplayMode(BLACK_AND_WHITE);

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    lv_obj_t *lbl_error = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(lbl_error, lv_color_hex(0x000000), 0);
    lv_label_set_text(lbl_error, "");
    lv_obj_align(lbl_error, LV_ALIGN_BOTTOM_LEFT, 10, -10);

    if (!display.apds.init()) {
        lv_label_set_text(lbl_error, "Can't init APDS9960!");
        lv_refr_now(lv_display_get_default());
        display.display();
        return;
    }

    display.apds.enableProximitySensor(false);
    display.apds.setProximityGain(PGAIN_2X);
    display.apds.enableGestureSensor(true);
    display.apds.setGestureGain(GGAIN_1X);
    display.apds.enableLightSensor(false);

    lv_obj_t *lbl_title = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(lbl_title, lv_color_hex(0x000000), 0);
    lv_label_set_text(lbl_title, "APDS9960 Sensor");
    lv_obj_align(lbl_title, LV_ALIGN_TOP_MID, 0, 20);

    lv_obj_t *lbl_proximity = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(lbl_proximity, lv_color_hex(0x000000), 0);
    lv_label_set_text(lbl_proximity, "Proximity: 0");
    lv_obj_set_pos(lbl_proximity, 20, 100);

    lv_obj_t *lbl_gesture = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(lbl_gesture, lv_color_hex(0x000000), 0);
    lv_label_set_text(lbl_gesture, "Gesture: -");
    lv_obj_set_pos(lbl_gesture, 20, 200);

    lv_obj_t *lbl_ambient = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(lbl_ambient, lv_color_hex(0x000000), 0);
    lv_label_set_text(lbl_ambient, "Ambient: 0");
    lv_obj_set_pos(lbl_ambient, 20, 300);

    lv_obj_t *lbl_color = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(lbl_color, lv_color_hex(0x000000), 0);
    lv_obj_set_width(lbl_color, 560);
    lv_label_set_long_mode(lbl_color, LV_LABEL_LONG_WRAP);
    lv_label_set_text(lbl_color, "R: 0  G: 0  B: 0");
    lv_obj_set_pos(lbl_color, 20, 400);

    lv_refr_now(lv_display_get_default());
    display.display();

    int updateCount = 0;
    char buf[128];

    while (1) {
        uint8_t prox = 0;
        display.apds.readProximity(prox);
        snprintf(buf, sizeof(buf), "Proximity: %u", prox);
        lv_label_set_text(lbl_proximity, buf);

        if (display.apds.isGestureAvailable()) {
            int g = display.apds.readGesture();
            const char *gStr = "-";
            switch (g) {
                case DIR_UP:    gStr = "Up";    break;
                case DIR_DOWN:  gStr = "Down";  break;
                case DIR_LEFT:  gStr = "Left";  break;
                case DIR_RIGHT: gStr = "Right"; break;
                default: break;
            }
            snprintf(buf, sizeof(buf), "Gesture: %s", gStr);
            lv_label_set_text(lbl_gesture, buf);
        }

        uint16_t ambient = 0, r = 0, g2 = 0, b2 = 0;
        display.apds.readAmbientLight(ambient);
        display.apds.readRedLight(r);
        display.apds.readGreenLight(g2);
        display.apds.readBlueLight(b2);

        snprintf(buf, sizeof(buf), "Ambient: %u", ambient);
        lv_label_set_text(lbl_ambient, buf);

        snprintf(buf, sizeof(buf), "R: %u  G: %u  B: %u", r, g2, b2);
        lv_label_set_text(lbl_color, buf);

        lv_refr_now(lv_display_get_default());
        if (++updateCount >= FULL_REFRESH_EVERY) {
            display.display();
            updateCount = 0;
        } else {
            display.partialUpdate();
        }

        vTaskDelay(pdMS_TO_TICKS(250));
    }
}
