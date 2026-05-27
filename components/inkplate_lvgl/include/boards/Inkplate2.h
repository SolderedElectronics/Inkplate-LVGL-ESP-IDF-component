/**
 * @file Inkplate2.h
 * @author Fran Fodor for Soldered
 * @brief Driver for Inkplate 2 board.
 *
 * https://github.com/SolderedElectronics/Inkplate-Esp-library
 * For more info about the product, please check:
 * https://docs.soldered.com/inkplate/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "esp_err.h"
#include "esp_rom_sys.h"
#include "soc/gpio_reg.h"
#include "soc/gpio_struct.h"

#include "BoardBase.h"
#include "GraphicsDefs.h"
#include "SPI.h"
#include "ditherAlgorithm.h"

#define EPAPER_RST_PIN GPIO_NUM_19
#define EPAPER_DC_PIN GPIO_NUM_33
#define EPAPER_CS_PIN GPIO_NUM_27
#define EPAPER_BUSY_PIN GPIO_NUM_32
#define EPAPER_CLK GPIO_NUM_18
#define EPAPER_DIN GPIO_NUM_23

#define BUSY_TIMEOUT_MS 1000

#define INKPLATE2_WHITE 0
#define INKPLATE2_BLACK 1
#define INKPLATE2_RED 2

#ifndef _swap_int16_t
#define _swap_int16_t(a, b)                                                    \
  {                                                                            \
    int16_t t = a;                                                             \
    a = b;                                                                     \
    b = t;                                                                     \
  }
#endif

#define E_INK_WIDTH 104
#define E_INK_HEIGHT 212

/**
 * @brief LVGL flush callback for Inkplate 2. Converts RGB565 pixels to
 *        WHITE / BLACK / RED and writes them into the framebuffer via
 *        writePixelInternal, then signals LVGL that flushing is done.
 */
void display_flush_callback(lv_display_t *disp, const lv_area_t *area,
                            uint8_t *px_map);

/**
 * @brief Class for Inkplate 2.
 *
 */
class Inkplate2 : public BoardBase {
public:
  /**
   * @brief Construct a new Inkplate 2 object.
   *
   */
  Inkplate2(lv_display_render_mode_t mode = LV_DISPLAY_RENDER_MODE_FULL);

  lv_display_t *getDisplay() { return m_disp; }

  void enableDithering(bool state) { m_ditherEnabled = state; }

  /**
   * @brief Writes a single pixel to the framebuffer.
   *
   * @param x x coordinate.
   * @param y y coordinate.
   * @param color INKPLATE2_WHITE, INKPLATE2_BLACK, or INKPLATE2_RED.
   */
  void writePixelInternal(int16_t x, int16_t y, uint16_t color);

  /**
   * @brief Sends both framebuffer planes to the panel and triggers a refresh.
   *
   * @param leaveOn if true, the panel is left powered on after refresh.
   * @return esp_err_t ESP_OK
   */
  esp_err_t display(bool leaveOn = false);

  /**
   * @brief Fill the framebuffer with white (erase all content).
   *
   */
  void clearDisplay();

  /**
   * @brief Fill the framebuffer with black (all pixels on).
   */
  void fillDisplay();

  /**
   * @brief Inkplate 2 does not support display mode switching.
   */
  void setDisplayMode(displayMode_t mode) override {}

  /**
   * @brief Power is managed internally per display() call.
   *
   * @return esp_err_t ESP_OK
   */
  esp_err_t einkOn() override { return ESP_OK; }

  /**
   * @brief Power is managed internally per display() call.
   *
   * @return esp_err_t ESP_OK
   */
  esp_err_t einkOff() override { return ESP_OK; }

  /**
   * @brief Inkplate 2 does not support partial updates.
   */
  void setFullUpdateThreshold(uint16_t numberOfPartialUpdates) override {}

  uint8_t m_displayMode = 0;

  uint8_t *m_framebufferColor = nullptr;
  uint8_t m_einkHeight = E_INK_HEIGHT;
  uint8_t m_einkWidth = E_INK_WIDTH;

private:
  /**
   * @brief Polls the busy pin until the panel is ready or the timeout expires.
   *
   * @param timeout maximum wait time in milliseconds.
   * @return bool true if the panel became ready, false on timeout.
   */
  bool waitForEpd(uint32_t timeout);

  /**
   * @brief Issues a hardware reset pulse to the panel.
   *
   */
  void resetPanel();

  /**
   * @brief Sends a single command byte to the panel.
   *
   * @param command command byte to send.
   */
  void sendCommand(uint8_t command);

  /**
   * @brief Sends a data buffer to the panel in 4092-byte chunks.
   *
   * @param data pointer to the data buffer.
   * @param n number of bytes to send.
   */
  void sendData(uint8_t *data, int n);

  /**
   * @brief Sends a single data byte to the panel.
   *
   * @param data byte to send.
   */
  void sendData(uint8_t data);

  /**
   * @brief Wakes or puts the panel into deep sleep.
   *
   * @param state true to enter deep sleep, false to wake.
   * @return bool true on success, false if the busy timeout expired during
   * wake.
   */
  bool setPanelDeepSleep(bool state);

  friend void display_flush_callback(lv_display_t *disp, const lv_area_t *area,
                                     uint8_t *px_map);

  SPI m_spi;
  uint8_t *m_lvglBuf = nullptr;
  lv_display_t *m_disp = nullptr;
  DitherAlgorithm m_dither;
  bool m_ditherEnabled = false;
};