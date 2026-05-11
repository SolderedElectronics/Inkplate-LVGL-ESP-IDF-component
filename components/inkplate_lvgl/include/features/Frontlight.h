/**
 * @file Frontlight.h
 * @author Fran Fodor for Soldered
 * @brief Driver for frontlight.
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
#include "stdint.h"

#include "I2C.h"
#include "PCAL.h"

#define FRONTLIGHT_I2C_ADDR 0x5C >> 1

/**
 * @brief Class for frontlight.
 *
 */
class Frontlight {
public:
  /**
   * @brief Construct a new Frontlight object.
   *
   */
  Frontlight() = default;

  /**
   * @brief Register the Frontlight on the I2C bus.
   *
   * @param i2c i2c instance.
   * @param expander io expander instance.
   * @param pin frontlight pin.
   * @return esp_err_t i2c error code.
   */
  esp_err_t begin(I2C &i2c, PCAL &expander, IOPin_t pin);

  /**
   * @brief Set frontlight brightness.
   *
   * @param value brightness value (0-63).
   * @return esp_err_t i2c error code.
   */
  esp_err_t setBrightness(uint8_t value);

  /**
   * @brief Set frontlight state (on/off).
   *
   * @param enable true to turn frontlight on.
   */
  void setState(bool enable);

private:
  PCAL *m_expander = nullptr;
  IOPin_t m_pin;
  i2c_master_dev_handle_t m_devHandle = NULL;
};