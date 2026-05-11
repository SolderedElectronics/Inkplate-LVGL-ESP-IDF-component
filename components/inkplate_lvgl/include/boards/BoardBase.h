/**
 * @file BoardBase.h
 * @author Fran Fodor for Soldered
 * @brief Collection of functions each board should have.
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

#include "I2S.h"
#include "esp_err.h"
#include <stdint.h>

typedef enum {
  BLACK_AND_WHITE = 0,
  GRAYSCALE,
} displayMode_t;

#if defined(CONFIG_INKPLATE_BOARD_INKPLATE6) ||                                \
    defined(CONFIG_INKPLATE_BOARD_INKPLATE6FLICK)
class BoardBase : public I2S
#else
class BoardBase
#endif
{
public:
  virtual void setDisplayMode(displayMode_t mode) = 0;
  virtual void writePixelInternal(int16_t x, int16_t y, uint16_t color) = 0;
  virtual void clearDisplay() = 0;
  virtual void fillDisplay() = 0;
  virtual esp_err_t display(bool leaveOn = false) = 0;
  virtual esp_err_t einkOn() = 0;
  virtual esp_err_t einkOff() = 0;
  virtual void setFullUpdateThreshold(uint16_t numberOfPartialUpdates) = 0;

  uint8_t getRotation() { return m_rotation; }
  void setRotation(uint8_t rot) { m_rotation = rot; }

  virtual ~BoardBase() = default;

protected:
  uint8_t m_rotation = 0;
};