/**
 * @file Inkplate6.h
 * @author Fran Fodor for Soldered
 * @brief Driver for Inkplate 6 and Inkplate 6 FLICK boards.
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

#include "BoardCommon.h"
#include "GraphicsDefs.h"

#include "PCAL.h"
#include "RTC.h"

extern PCAL expander1;

#define IO_INT_ADDR 0x20
#define IO_EXT_ADDR 0x21

#if CONFIG_INKPLATE_BOARD_INKPLATE6
#define SD_PMOS_PIN IO_NUM_B2
#define E_INK_WIDTH 800
#define E_INK_HEIGHT 600
static const uint8_t waveform3Bit[8][9] = {
    {0, 0, 0, 0, 1, 1, 1, 1, 0}, {0, 0, 0, 1, 1, 1, 1, 0, 0},
    {1, 1, 1, 1, 0, 2, 1, 0, 0}, {1, 1, 1, 2, 2, 1, 1, 0, 0},
    {1, 1, 1, 1, 2, 2, 1, 0, 0}, {0, 1, 1, 1, 2, 2, 1, 0, 0},
    {0, 0, 0, 0, 1, 1, 2, 0, 0}, {0, 0, 0, 0, 0, 0, 2, 0, 0}};
#elif CONFIG_INKPLATE_BOARD_INKPLATE6FLICK
#include "Frontlight.h"
#include "Touchscreen.h"
#define SD_PMOS_PIN IO_NUM_B5
#define E_INK_WIDTH 1024
#define E_INK_HEIGHT 758
#define FRONTLIGHT_EN IO_NUM_B3

#define TOUCHSCREEN_EN IO_NUM_B4
#define TOUCHSCREEN_RST IO_NUM_B2
#define TOUCHSCREEN_INT GPIO_NUM_36
#define TOUCHSCREEN_IO_EXPANDER IO_INT_ADDR
#define TOUCHSCREEN_IO_REGS ioRegsInt
static const uint8_t waveform3Bit[8][9] = {
    {0, 0, 0, 0, 0, 1, 1, 1, 0}, {0, 0, 1, 2, 1, 1, 2, 1, 0},
    {0, 1, 1, 2, 1, 1, 1, 2, 0}, {1, 1, 1, 2, 2, 1, 1, 2, 0},
    {1, 1, 1, 2, 1, 2, 1, 2, 0}, {0, 1, 1, 2, 1, 2, 1, 2, 0},
    {1, 2, 1, 1, 2, 2, 1, 2, 0}, {0, 0, 0, 0, 0, 0, 0, 2, 0}};
#endif

/**
 * @brief Class for Inkplate 6.
 *
 */
class Inkplate6 : public BoardCommon {
public:
  /**
   * @brief Construct a new Inkplate 6 object
   *
   */
  Inkplate6();

  /**
   * @brief Send only the changed pixels to the display (1-bit mode only).
   *
   * @param forced if true, bypasses the partial update block flag.
   * @param leaveOn if true, leaves the e-ink panel powered on after the update.
   * @return uint32_t number of pixels that changed; 0 if a full update was
   * performed instead.
   */
  uint32_t partialUpdate(bool forced = false, bool leaveOn = false);

  /**
   * @brief Power on the e-ink panel and assert all required control signals.
   *
   * @return esp_err_t error code.
   */
  esp_err_t einkOn() override;

  /**
   * @brief Power off the e-ink panel and tri-state all data lines.
   *
   * @return esp_err_t error code.
   */
  esp_err_t einkOff() override;

  RTC rtc;
#if CONFIG_INKPLATE_BOARD_INKPLATE6FLICK
  Frontlight frontlight;
  TouchCypress touchscreen;
#endif

private:
  /**
   * @brief Allocate framebuffers.
   *
   * @return esp_err_t error code.
   */
  esp_err_t initBuffers();

  /**
   * @brief Pre-compute the grayscale waveform lookup tables.
   *
   */
  void calculateLUTs();

  /**
   * @brief Drive the panel using 3-bit (8-level grayscale) waveform.
   *
   * @param leaveOn if true, leave the panel powered on after the update.
   * @return esp_err_t error code
   */
  esp_err_t display3b(bool leaveOn);

  /**
   * @brief Drive the panel using 1-bit (black and white) waveform.
   *
   * @param leaveOn if true, leave the panel powered on after the update.
   * @return esp_err_t error code
   */
  esp_err_t display1b(bool leaveOn);

  /**
   * @brief Initialize all GPIO pins and IO expander directions.
   *
   */
  void gpioInit();

  /**
   * @brief Run a single cleaning pass on the panel.
   *
   * @param c pixel value to write (0 = black, 1 = white, 2 = discharge, 3 =
   * skip).
   * @param rep number of times to repeat the pass.
   */
  void clean(uint8_t c, uint8_t rep);

  /**
   * @brief  Set all e-ink data and control GPIOs to output mode.
   */
  void pinsAsOutputs();

  /**
   * @brief  Set all e-ink data and control GPIOs to high-impedance (input)
   * mode.
   */
  void pinsZstate();

  uint8_t *m_glut = nullptr;
  uint8_t *m_glut2 = nullptr;
  uint32_t *m_pinLUT = nullptr;
};
