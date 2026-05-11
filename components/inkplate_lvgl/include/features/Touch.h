/**
 * @file Touch.h
 * @author Fran Fodor for Soldered
 * @brief Abstract class for touchscreen drivers.
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

#include "I2C.h"
#include "PCAL.h"
#include "driver/i2c_master.h"
#include <stdint.h>

/**
 * @brief Abstract class for touchscreen drivers.
 *
 */
class Touch {
public:
  virtual ~Touch() = default;

  virtual esp_err_t begin(I2C &i2c, PCAL &expander, uint8_t powerState) = 0;
  virtual void shutdown() = 0;
  virtual bool available() = 0;
  virtual bool touchInArea(int16_t x1, int16_t y1, int16_t w, int16_t h) = 0;
  virtual uint8_t getData(uint16_t *xPos, uint16_t *yPos,
                          uint8_t *z = nullptr) = 0;
  virtual void getRawData(uint8_t *b) = 0;
  virtual void setPowerState(uint8_t s) = 0;
  virtual uint8_t getPowerState() = 0;

  // Shared state accessed by drivers
  uint8_t touchN = 0;
  uint16_t touchX[2] = {0, 0};
  uint16_t touchY[2] = {0, 0};
  uint32_t touchT = 0;
  bool m_tsInitDone = false;

protected:
  PCAL *m_expander = nullptr;
  i2c_master_dev_handle_t m_devHandle = nullptr;
};