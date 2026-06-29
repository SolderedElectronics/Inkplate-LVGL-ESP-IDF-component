/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       External I/O expander example for Soldered Inkplate 13 with LVGL.
 *
 * @details     Demonstrates using the onboard PCAL6416A I/O expander by blinking
 *              an LED connected to pin P1-7 (IO_NUM_B7). Connect a 330 Ohm
 *              resistor to P1-7 on the IO Expander header, the other end to the
 *              LED anode, and the cathode to GND.
 *
 *              Pin mapping:
 *              P0-0 = IO_NUM_A0 = 0, ..., P0-7 = IO_NUM_A7 = 7
 *              P1-0 = IO_NUM_B0 = 8, ..., P1-7 = IO_NUM_B7 = 15
 *
 * Requirements:
 * - Board:      Soldered Inkplate 13
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 13, USB cable, LED + 330 Ohm resistor
 * - Extra:      None
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate13
 *
 * How to use:
 * 1) Connect LED + resistor to P1-7 and GND.
 * 2) Build and flash. LED blinks every second.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 */

#include "sdkconfig.h"

#ifndef CONFIG_INKPLATE_BOARD_INKPLATE13
#error                                                                         \
    "Wrong board selection for this example, please select Inkplate13 in the boards menu."
#endif

#include "Inkplate.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);

    display.expander1.setDirection(IO_NUM_B7, IO_MODE_OUTPUT);

    while (1) {
        display.expander1.setLevel(IO_NUM_B7, 0); // LED off
        vTaskDelay(pdMS_TO_TICKS(1000));
        display.expander1.setLevel(IO_NUM_B7, 1); // LED on
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
