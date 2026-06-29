/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       External I/O expander example for Soldered Inkplate 6Color with LVGL.
 *
 * @details     Demonstrates using the onboard MCP23017 I/O expander by toggling
 *              all 16 pins (P0-0 to P1-7) between LOW and HIGH every second.
 *              Connect an LED with a 330 Ohm resistor to any expander pin to see
 *              it blink.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6Color
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 6Color, USB cable, LED + 330 Ohm resistor (optional)
 * - Extra:      None
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate6 Color
 *
 * How to use:
 * 1) Build and flash to Inkplate 6Color.
 * 2) All expander pins toggle every 1 second.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 */

#include "sdkconfig.h"

#ifndef CONFIG_INKPLATE_BOARD_INKPLATE6COLOR
#error \
    "Wrong board selection for this example, please select Inkplate6Color in the boards menu."
#endif

#include "Inkplate.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_PARTIAL);

    for (int i = 0; i < 16; i++) {
        expander1.setDirection((IOPin_t)i, IO_MODE_OUTPUT);
    }

    while (1) {
        for (int i = 0; i < 16; i++) {
            expander1.setLevel((IOPin_t)i, 0);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));

        for (int i = 0; i < 16; i++) {
            expander1.setLevel((IOPin_t)i, 1);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
