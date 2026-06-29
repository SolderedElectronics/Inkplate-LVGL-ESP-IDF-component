/**
 * @file        main.cpp
 * @author      Fran Fodor for Soldered
 * @brief       Burn-in clean example for Soldered Inkplate 2 with LVGL.
 *
 * @details     Attempts to reduce e-paper burn-in by repeatedly refreshing
 *              the display with a blank white frame. After all cycles complete,
 *              a "Clearing Done!" message is shown on screen.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 2
 * - Framework:  ESP-IDF v6.x
 * - Hardware:   Inkplate 2, USB cable
 * - Extra:      None
 *
 * Configuration:
 * - Menuconfig -> Inkplate Boards -> Inkplate2
 *
 * How to use:
 * 1) Adjust CLEAR_CYCLES and CYCLES_DELAY_MS as needed.
 * 2) Build and flash to Inkplate 2.
 * 3) Wait for all cycles to complete.
 *
 * Expected output:
 * - Display flashes white CLEAR_CYCLES times, then shows "Clearing Done!".
 *
 * Notes:
 * - lv_refr_now() renders the LVGL scene into the framebuffer.
 * - display.display() must be called to update the physical e-paper panel.
 *
 * Docs:         https://docs.soldered.com/inkplate
 * Support:      https://forum.soldered.com/
 */

#include "sdkconfig.h"

#ifndef CONFIG_INKPLATE_BOARD_INKPLATE2
#error                                                                         \
    "Wrong board selection for this example, please select Inkplate2 in the boards menu."
#endif

#include "Inkplate.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define CLEAR_CYCLES     5
#define CYCLES_DELAY_MS  5000

extern "C" void app_main(void) {
    Inkplate display(LV_DISPLAY_RENDER_MODE_FULL);
    display.clearDisplay();

    int cycles = CLEAR_CYCLES;
    while (cycles--) {
        lv_refr_now(lv_display_get_default()); // Render blank frame into framebuffer
        display.display();                     // Push to e-paper panel
        vTaskDelay(pdMS_TO_TICKS(CYCLES_DELAY_MS));
    }

    lv_obj_t *screen = lv_scr_act();
    lv_obj_t *label = lv_label_create(screen);
    lv_label_set_text(label, "Clearing Done!");
    lv_obj_set_style_text_font(label, &lv_font_montserrat_14, 0);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 10);

    lv_refr_now(lv_display_get_default()); // Render LVGL scene into framebuffer
    display.display();                     // Push framebuffer to e-paper panel
}
