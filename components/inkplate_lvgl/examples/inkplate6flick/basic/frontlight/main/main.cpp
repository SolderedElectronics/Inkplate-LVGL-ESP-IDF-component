/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       Frontlight control example for Soldered Inkplate 6Flick with LVGL.
 *
 * @details     Demonstrates interactive frontlight brightness control via UART0.
 *              Send '+' to increase brightness, '-' to decrease it, or 's' to
 *              trigger a light show. Current brightness is shown on the display.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6Flick
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 6Flick, USB cable
 * - Extra:      Serial terminal at 115200 baud connected to UART0
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate6 Flick
 *
 * How to use:
 * 1) Build and flash to Inkplate 6Flick.
 * 2) Open a serial terminal at 115200 baud.
 * 3) Send '+' to increase, '-' to decrease brightness, 's' for light show.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 */

#include "sdkconfig.h"

#ifndef CONFIG_INKPLATE_BOARD_INKPLATE6FLICK
#error \
    "Wrong board selection for this example, please select Inkplate6Flick in the boards menu."
#endif

#include "Inkplate.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

#define UART_BUF_SIZE 256

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_PARTIAL);
    display.setDisplayMode(BLACK_AND_WHITE);

    uart_config_t uart_config = {};
    uart_config.baud_rate  = 115200;
    uart_config.data_bits  = UART_DATA_8_BITS;
    uart_config.parity     = UART_PARITY_DISABLE;
    uart_config.stop_bits  = UART_STOP_BITS_1;
    uart_config.flow_ctrl  = UART_HW_FLOWCTRL_DISABLE;
    uart_config.source_clk = UART_SCLK_DEFAULT;
    uart_driver_install(UART_NUM_0, UART_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_0, &uart_config);

    int b = 31;

    display.frontlight.setState(true);
    display.frontlight.setBrightness(b);

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    lv_obj_t *label = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_color(label, lv_color_hex(0x000000), 0);
    lv_obj_center(label);

    char buf[128];
    snprintf(buf, sizeof(buf),
             "Frontlight: %d/63\n\nSend '+' / '-' to adjust\n's' for light show", b);
    lv_label_set_text(label, buf);

    lv_refr_now(lv_display_get_default());
    display.display();

    while (1) {
        uint8_t data[1];
        int len = uart_read_bytes(UART_NUM_0, data, sizeof(data), pdMS_TO_TICKS(20));
        if (len > 0) {
            bool change = false;
            char c = (char)data[0];

            if (c == '+' && b < 63) { b++; change = true; }
            if (c == '-' && b > 0)  { b--; change = true; }

            if (c == 's') {
                for (int j = 0; j < 4; j++) {
                    for (int i = 0; i < 64; i++) {
                        display.frontlight.setBrightness(i);
                        vTaskDelay(pdMS_TO_TICKS(30));
                    }
                    for (int i = 63; i >= 0; i--) {
                        display.frontlight.setBrightness(i);
                        vTaskDelay(pdMS_TO_TICKS(30));
                    }
                }
                change = true;
            }

            if (change) {
                display.frontlight.setBrightness(b);
                snprintf(buf, sizeof(buf),
                         "Frontlight: %d/63\n\nSend '+' / '-' to adjust\n's' for light show", b);
                lv_label_set_text(label, buf);
                lv_obj_center(label);
                lv_refr_now(lv_display_get_default());
                display.partialUpdate();
            }
        }
    }
}
