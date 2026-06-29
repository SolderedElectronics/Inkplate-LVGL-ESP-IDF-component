/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       VCOM voltage setup example for Soldered Inkplate 10 with LVGL.
 *
 * @details     WARNING: VCOM voltage is stored in the TPS65186 PMIC EEPROM and
 *              in ESP32 NVS. The PMIC EEPROM supports a limited number of write
 *              cycles, so do NOT run this example repeatedly. Set VCOM once.
 *
 *              Displays 8 grayscale shades for visual quality assessment.
 *              Enter the VCOM value via serial to program it.
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
 * 2) Open a serial terminal at 115200 baud.
 * 3) Enter VCOM voltage (e.g. -1.23) and press Enter.
 * 4) VCOM is written to PMIC EEPROM and NVS; display refreshes.
 *
 * Notes:
 * - VCOM must be in the range [-5.0, 0.0] volts.
 * - The optimal VCOM is sometimes printed on the flat cable connector.
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
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

#define UART_BUF_SIZE 256

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);
    display.setDisplayMode(GRAYSCALE);

    uart_config_t uart_config = {};
    uart_config.baud_rate = 115200;
    uart_config.data_bits = UART_DATA_8_BITS;
    uart_config.parity    = UART_PARITY_DISABLE;
    uart_config.stop_bits = UART_STOP_BITS_1;
    uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    uart_driver_install(UART_NUM_0, UART_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_0, &uart_config);

    auto displayTestImage = [&display]() {
        double currentVCOM = display.getStoredVCOM();

        lv_obj_clean(lv_screen_active());
        lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xFFFFFF), LV_PART_MAIN);

        char buf[32];
        snprintf(buf, sizeof(buf), "Current VCOM: %.2f", currentVCOM);

        lv_obj_t *label = lv_label_create(lv_screen_active());
        lv_label_set_text(label, buf);
        lv_obj_set_style_text_color(label, lv_color_hex(0x000000), 0);
        lv_obj_align(label, LV_ALIGN_TOP_LEFT, 5, 5);

        lv_display_t *d = lv_display_get_default();
        int w = lv_display_get_horizontal_resolution(d);
        int h = lv_display_get_vertical_resolution(d);

        for (int i = 0; i < 8; i++) {
            uint8_t v = (uint8_t)((i * 255) / 7);
            lv_color_t color = lv_color_make(v, v, v);

            lv_obj_t *rect = lv_obj_create(lv_screen_active());
            lv_obj_remove_flag(rect, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_size(rect, w / 8, h - 40);
            lv_obj_set_pos(rect, (w / 8) * i, 40);
            lv_obj_set_style_bg_color(rect, color, 0);
            lv_obj_set_style_border_width(rect, 0, 0);
            lv_obj_set_style_radius(rect, 0, 0);
            lv_obj_set_style_pad_all(rect, 0, 0);
        }

        lv_refr_now(lv_display_get_default());
        display.display();
    };

    displayTestImage();

    printf("\nWARNING: VCOM is written to PMIC EEPROM (limited write cycles).\n");
    printf("The optimal VCOM is sometimes printed on the flat cable connector.\n");
    printf("Enter VCOM in range [-5.0, 0.0], e.g. -1.23, then press Enter.\n\n");

    while (1) {
        printf("Enter VCOM: ");
        fflush(stdout);

        char buf[32] = {};
        int idx = 0;
        uint8_t c;

        while (idx < 31) {
            if (uart_read_bytes(UART_NUM_0, &c, 1, pdMS_TO_TICKS(10000)) == 1) {
                if (c == '\n' || c == '\r') break;
                buf[idx++] = (char)c;
            } else {
                break;
            }
        }

        double vcom = 0.0;
        if (sscanf(buf, "%lf", &vcom) != 1 || vcom < -5.0 || vcom > 0.0) {
            printf("Invalid: \"%s\" — must be in [-5.0, 0.0]\n\n", buf);
            continue;
        }

        printf("Setting VCOM to %.2f...\n", vcom);
        if (display.setVCOM(vcom) == ESP_OK) {
            printf("VCOM set successfully.\n\n");
        } else {
            printf("VCOM write failed.\n\n");
            continue;
        }

        displayTestImage();
    }
}
