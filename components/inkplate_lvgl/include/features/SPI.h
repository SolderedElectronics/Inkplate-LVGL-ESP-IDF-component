/**
 * @file SPI.h
 * @author Fran Fodor for Soldered
 * @brief Helper for SPI communication.
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

#include "driver/gpio.h"
#include "driver/spi_master.h"

/**
 * @brief Class for SPI communication.
 *
 */
class SPI {
public:
  /**
   * @brief Construct a new SPI object.
   *
   * @param mosi mosi pin.
   * @param clk clk pin.
   */
  SPI(gpio_num_t mosi, gpio_num_t clk);

  /**
   * @brief Initializes the SPI bus and registers the device.
   *
   */
  void init();

  /**
   * @brief Deinitializes the SPI bus and releases resources.
   *
   */
  void deinit();
  /**
   * @brief Check if SPI initialized.
   *
   * @return bool true if initialized.
   */
  bool isInitialized() const { return m_spiDev != nullptr; }

  /**
   * @brief Sends a single command byte over SPI.
   *
   * @param command command byte to send.
   * @param dcPin DC pin to control command/data selection.
   */
  void sendCommand(uint8_t command, gpio_num_t dcPin);

  /**
   * @brief Sends a data buffer over SPI in chunks.
   *
   * @param data pointer to data buffer.
   * @param n number of bytes to send.
   * @param dcPin DC pin to control command/data selection.
   *
   * @note Transmits in 4092 byte chunks to respect DMA limits. Waits 1 ms after
   *       the last chunk.
   */
  void sendData(uint8_t *data, int n, gpio_num_t dcPin);

  /**
   * @brief Sends a single data byte over SPI.
   *
   * @param data byte to send.
   * @param dcPin DC pin to control command/data selection.
   */
  void sendData(uint8_t data, gpio_num_t dcPin);

private:
  gpio_num_t m_mosi;
  gpio_num_t m_clk;
  spi_host_device_t m_host;
  spi_device_handle_t m_spiDev = nullptr;
};
