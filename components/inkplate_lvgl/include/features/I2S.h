/**
 * @file I2S.h
 * @author Fran Fodor for Soldered
 * @brief Helper for I2S communication.
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

#include "esp_attr.h"
#include "esp_rom_lldesc.h"
#include "soc/i2s_struct.h"

/**
 * @brief Class for I2S communication.
 *
 */
class I2S {
public:
  /**
   * @brief Construct a new I2S object.
   *
   * @param clockDivider I2S master clock divider; BCK frequency = 80 MHz /
   * clockDivider.
   *
   * @note Acquires I2S1 peripheral, resets FIFO/DMA/TX/RX, and configures
   *       LCD parallel mode for EPD data output.
   */
  IRAM_ATTR I2S(uint8_t clockDivider = 5);

  /**
   * @brief Transmit one DMA descriptor's worth of data over I2S to the EPD.
   *
   * @note Resets the FIFO, DMA, and TX module before each transfer.
   *       Blocks until the DMA out_total_eof interrupt fires.
   */
  void IRAM_ATTR sendDataI2S();

  /**
   * @brief Route an I2S1 signal to a GPIO pin via the GPIO matrix.
   *
   * @param pin GPIO number (0–39).
   * @param function I2S signal index from gpio_sig_map.h (e.g.
   * I2S1O_DATA_OUT0_IDX).
   * @param inv set to 1 to invert the output signal, 0 for normal polarity.
   */
  void IRAM_ATTR setI2S1pin(uint32_t pin, uint32_t function, uint32_t inv);

protected:
  volatile uint8_t *m_dmaLineBuffer;
  volatile lldesc_s *m_dmaI2SDesc;

  // use only I2S1 (I2S0 is not compatible with 8 bit data).
  volatile i2s_dev_t *m_i2s;
};
