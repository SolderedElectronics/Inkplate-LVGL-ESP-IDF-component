/**
 * @file I2C.h
 * @author Fran Fodor for Soldered
 * @brief Helper for I2C communication.
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

#include "driver/i2c_master.h"

#ifndef CONFIG_INKPLATE_BOARD_INKPLATE13
#define I2C_SDA GPIO_NUM_21
#define I2C_SCL GPIO_NUM_22
#else
#define I2C_SDA GPIO_NUM_8
#define I2C_SCL GPIO_NUM_9
#endif

/**
 * @brief Class for I2C communication.
 *
 */
class I2C {
public:
  /**
   * @brief Construct a new I2C object.
   *
   */
  I2C();

  /**
   * @brief Add a device to I2C bus.
   *
   * @param addr i2c address.
   * @param handle device handle.
   * @return esp_err_t i2c error code.
   */
  esp_err_t addDevice(uint8_t addr, i2c_master_dev_handle_t *handle);

  /**
   * @brief Get bus handle.
   *
   * @return i2c_master_bus_handle_t bus handle.
   */
  i2c_master_bus_handle_t getBusHandle() { return m_busHandle; }

private:
  i2c_master_bus_handle_t m_busHandle;
};