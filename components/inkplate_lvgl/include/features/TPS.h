/**
 * @file TPS.h
 * @author Fran Fodor for Soldered
 * @brief Driver for TPS65186.
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

#define TPS_I2C_ADDR 0x48

#define TPS_PWR_GOOD 0b11111010
#define TPS_PWRUP_SEQ 0b11100100
#define TPS_PWRDN_SEQ 0b00011011

// IO expander pin wired to TPS65186 INT output
#define TPS_INT_PIN IO_NUM_A6

/**
 * @brief Class for TPS65186.
 *
 */
class TPS {
public:
  /**
   * @brief Construct a new TPS object.
   *
   * @param i2c i2c instance.
   */
  TPS(I2C &i2c);

  /**
   * @brief Write startup power-up/down sequences to the PMIC.
   *
   * @return esp_err_t i2c error code.
   *
   * @note Should be called once, with WAKEUP asserted.
   */
  esp_err_t initSequences();

  /**
   * @brief Enable all power rails (register 0x01, bit 5).
   *
   * @return esp_err_t i2c error code.
   */
  esp_err_t enableRails();

  /**
   * @brief Disable all power rails (register 0x01 = 0x00).
   *
   * @return esp_err_t i2c error code.
   */
  esp_err_t disableRails();

  /**
   * @brief Set the power-up sequence register (UPSEQ0, 0x09).
   *
   * @param seq sequence byte value.
   * @return esp_err_t i2c error code.
   */
  esp_err_t setPowerUpSequence(uint8_t seq);

  /**
   * @brief Set the power-down sequence register (DWNSEQ0, 0x0B).
   *
   * @param seq sequence byte value.
   * @return esp_err_t i2c error code.
   */
  esp_err_t setPowerDownSequence(uint8_t seq);

  /**
   * @brief Read the PGSTAT register (0x0F).
   *
   * @return uint8_t raw power-good status bitmask.
   */
  uint8_t readPowerGood();

  /**
   * @brief Poll the PMIC until power-good state matches target or 250 ms
   * timeout.
   *
   * @param target true = wait for rails up, false = wait for rails down.
   * @return bool true if target state reached, false if timeout elapsed.
   */
  bool waitPowerGood(bool target);

  /**
   * @brief Program VCOM voltage into the TPS65186 internal EEPROM.
   *
   * @param vcom VCOM value in volts (must be in range -5.0 to 0.0).
   * @param expander IO expander instance with TPS_INT_PIN connected to TPS65186
   * INT.
   * @return esp_err_t i2c error code.
   *
   * @note Call with eink power already on (einkOn()).
   */
  esp_err_t writeVCOM(double vcom, PCAL &expander);

  /**
   * @brief Read the VCOM voltage currently stored in TPS65186 registers.
   *
   * @return double VCOM in volts (negative, e.g. -1.23).
   *
   * @note Call with eink power already on (einkOn()).
   */
  double readVCOM();

  /**
   * @brief Read the on-chip thermistor temperature.
   *
   * @return int8_t temperature in degrees celsius.
   *
   * @note Call with eink power already on (einkOn()).
   */
  int8_t readTemperature();

private:
  /**
   * @brief Writes to register using I2C.
   *
   * @param reg register to write to.
   * @param val value to write.
   * @return esp_err_t i2c error code.
   */
  esp_err_t writeReg(uint8_t reg, uint8_t val);

  /**
   * @brief Reads from register using I2C.
   *
   * @param reg register to read from.
   * @return uint8_t read value.
   */
  uint8_t readReg(uint8_t reg);

  i2c_master_dev_handle_t m_handle = NULL;
};
