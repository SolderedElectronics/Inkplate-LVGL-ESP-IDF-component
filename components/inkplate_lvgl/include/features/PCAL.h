/**
 * @file PCAL.h
 * @author Fran Fodor for Soldered
 * @brief Driver for PCAL6416A IO expander.
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

// PCAL6416A registers
#define PCAL6416A_INPORT0 0x00
#define PCAL6416A_INPORT1 0x01
#define PCAL6416A_OUTPORT0 0x02
#define PCAL6416A_OUTPORT1 0x03
#define PCAL6416A_POLINVPORT0 0x04
#define PCAL6416A_POLINVPORT1 0x05
#define PCAL6416A_CFGPORT0 0x06
#define PCAL6416A_CFGPORT1 0x07
#define PCAL6416A_OUTDRVST_REG00 0x40
#define PCAL6416A_OUTDRVST_REG01 0x41
#define PCAL6416A_OUTDRVST_REG10 0x42
#define PCAL6416A_OUTDRVST_REG11 0x43
#define PCAL6416A_INLAT_REG0 0x44
#define PCAL6416A_INLAT_REG1 0x45
#define PCAL6416A_PUPDEN_REG0 0x46
#define PCAL6416A_PUPDEN_REG1 0x47
#define PCAL6416A_PUPDSEL_REG0 0x48
#define PCAL6416A_PUPDSEL_REG1 0x49
#define PCAL6416A_INTMSK_REG0 0x4A
#define PCAL6416A_INTMSK_REG1 0x4B
#define PCAL6416A_INTSTAT_REG0 0x4C
#define PCAL6416A_INTSTAT_REG1 0x4D
#define PCAL6416A_OUTPORT_CONF 0x4F

/**
 * @brief IO expander pin identifier.
 *
 */
typedef enum {
  IO_NUM_A0 = 0,
  IO_NUM_A1,
  IO_NUM_A2,
  IO_NUM_A3,
  IO_NUM_A4,
  IO_NUM_A5,
  IO_NUM_A6,
  IO_NUM_A7,
  IO_NUM_B0,
  IO_NUM_B1,
  IO_NUM_B2,
  IO_NUM_B3,
  IO_NUM_B4,
  IO_NUM_B5,
  IO_NUM_B6,
  IO_NUM_B7
} IOPin_t;

/**
 * @brief IO expander pin direction.
 *
 */
typedef enum {
  IO_MODE_INPUT = 0,
  IO_MODE_OUTPUT,
} IOMode_t;

/**
 * @brief IO expander internal pull resistor mode.
 *
 */
typedef enum {
  IO_PULLUP = 0,
  IO_PULLDOWN,
} IOPullMode_t;

/**
 * @brief GPIO port identifier.
 */
typedef enum {
  IO_PORT_0 = 0,
  IO_PORT_1,
} IOPort_t;

/**
 * @brief GPIO output driver mode.
 */
typedef enum {
  IO_PUSH_PULL = 0,
  IO_OPEN_DRAIN,
} IOOutputMode_t;

/**
 * @brief GPIO pin drive strength in milliamps.
 */
typedef enum {
  IO_DRIVE_25 = 0,
  IO_DRIVE_50,
  IO_DRIVE_75,
  IO_DRIVE_100,
} IODriveStrength_t;

/**
 * @brief Class for PCAL6416A IO expander.
 *
 */
class PCAL {
public:
  /**
   * @brief Construct a new PCAL object.
   *
   * @param addr i2c address.
   * @param i2c i2c instance.
   */
  PCAL(uint8_t addr, I2C &i2c);

  /**
   * @brief Set output level of a pin.
   *
   * @param pin pin to set.
   * @param level 0 for low, non-zero for high.
   * @param bypass set to true if you wish to bypass blocked pin,
   *               use with caution as setting a pin that is used by the display
   * could damage it.
   * @return esp_err_t i2c error code.
   */
  esp_err_t setLevel(IOPin_t pin, uint8_t level, bool bypass = false);

  /**
   * @brief Get the output level of a pin.
   *
   * @param pin pin to read.
   * @param bypass set to true if you wish to bypass blocked pin,
   *               use with caution as setting a pin that is used by the display
   * could damage it.
   * @return int value (level) of pin.
   */
  int getLevel(IOPin_t pin, bool bypass = false);

  /**
   * @brief Set output level of a port.
   *
   * @param port port to set.
   * @param value value to write to port.
   * @return esp_err_t i2c error code.
   */
  esp_err_t setPort(IOPort_t port, uint8_t value);

  /**
   * @brief Get input level of a port.
   *
   * @param port port to read.
   * @return int port value.
   */
  int getPort(IOPort_t port);

  /**
   * @brief Set direction of a port.
   *
   * @param port port to set.
   * @param mask direction bitmask, 1 for input 0 for output.
   * @return esp_err_t i2c error code.
   */
  esp_err_t setPortDirection(IOPort_t port, uint8_t mask);

  /**
   * @brief Set direction (input/output) of a pin.
   *
   * @param pin pin to configure.
   * @param mode IO_MODE_INPUT or IO_MODE_OUTPUT.
   * @param bypass set to true if you wish to bypass blocked pin,
   *               use with caution as setting a pin that is used by the display
   * could damage it.
   * @return esp_err_t i2c error code.
   */
  esp_err_t setDirection(IOPin_t pin, IOMode_t mode, bool bypass = false);

  /**
   * @brief Set pull mode of pin (up/down).
   *
   * @param pin pin to configure.
   * @param pullMode IO_PULLUP or IO_PULLDOWN.
   * @param bypass set to true if you wish to bypass blocked pin,
   *               use with caution as setting a pin that is used by the display
   * could damage it.
   * @return esp_err_t i2c error code.
   */
  esp_err_t setPullMode(IOPin_t pin, IOPullMode_t pullMode,
                        bool bypass = false);

  /**
   * @brief Set polarity inversion for a pin.
   *
   * @param pin pin to configure.
   * @param invert true to invert, false for normal.
   * @param bypass set to true if you wish to bypass blocked pin,
   *               use with caution as setting a pin that is used by the display
   * could damage it.
   * @return esp_err_t i2c error code.
   */
  esp_err_t setPolarityInversion(IOPin_t pin, bool invert, bool bypass = false);

  /**
   * @brief Set input latch for a pin.
   *
   * @param pin pin to configure.
   * @param latch true to enable latch, false to disable.
   * @param bypass set to true if you wish to bypass blocked pin,
   *               use with caution as setting a pin that is used by the display
   * could damage it.
   * @return esp_err_t i2c error code.
   *
   * @note When latched, the input state is held until read.
   */
  esp_err_t setInputLatch(IOPin_t pin, bool latch, bool bypass = false);

  /**
   * @brief Set output mode (push-pull or open-drain) for a port.
   *
   * @param port port to configure.
   * @param mode IO_PUSH_PULL or IO_OPEN_DRAIN.
   * @return esp_err_t i2c error code.
   */
  esp_err_t setOutputMode(IOPort_t port, IOOutputMode_t mode);

  /**
   * @brief Set output drive strength for a pin.
   *
   * @param pin pin to configure.
   * @param strength IO_DRIVE_25, IO_DRIVE_50, IO_DRIVE_75, or IO_DRIVE_100.
   * @param bypass set to true if you wish to bypass blocked pin,
   *               use with caution as setting a pin that is used by the display
   * could damage it.
   * @return esp_err_t i2c error code.
   *
   * @note Each pin occupies 2 bits in one of four drive strength registers
   *       (REG00–REG11). Port 0 uses 0x40/0x41, port 1 uses 0x42/0x43.
   */
  esp_err_t setDriveStrength(IOPin_t pin, IODriveStrength_t strength,
                             bool bypass = false);

  /**
   * @brief Enable interrupt for a pin.
   *
   * @param pin pin to enable interrupt on.
   * @param bypass set to true if you wish to bypass blocked pin,
   *               use with caution as setting a pin that is used by the display
   * could damage it.
   * @return esp_err_t i2c error code.
   */
  esp_err_t interruptEnable(IOPin_t pin, bool bypass = false);

  /**
   * @brief Disable interrupt for a pin.
   *
   * @param pin pin to disable interrupt on.
   * @param bypass set to true if you wish to bypass blocked pin,
   *               use with caution as setting a pin that is used by the display
   * could damage it.
   * @return esp_err_t i2c error code.
   */
  esp_err_t interruptDisable(IOPin_t pin, bool bypass = false);

  /**
   * @brief Check if an interrupt has been triggered on a pin.
   *
   * @param pin pin to check.
   * @return bool true if interrupt triggered.
   *
   * @note The interrupt status register clears automatically on read.
   */
  bool getInterrupt(IOPin_t pin);

  /**
   * @brief Block a pin from being modified.
   *
   * @param pin pin to block.
   * @return esp_err_t i2c error code.
   */
  esp_err_t blockPin(IOPin_t pin);

  /**
   * @brief Unblock a previously blocked pin.
   *
   * @param pin pin to unblock.
   * @return esp_err_t i2c error code.
   */
  esp_err_t unblockPin(IOPin_t pin);

private:
  /**
   * @brief Resolve register address and bit position for a pin.
   *
   * @param pin pin to resolve.
   * @param baseReg base register.
   * @param reg resolved register address.
   * @param bit bit position within the register.
   */
  void pinToRegBit(IOPin_t pin, uint8_t baseReg, uint8_t &reg, uint8_t &bit);

  /**
   * @brief Check if a pin is blocked.
   *
   * @param pin pin to check.
   * @return bool true if pin blocked
   */
  bool checkBlockedPins(IOPin_t pin);

  /**
   * @brief Internal function to write to a register.
   *
   * @param reg register to write to.
   * @param val value to write.
   * @return esp_err_t i2c error code.
   *
   * @note Tries to transmit indefinitely, until it succeeds.
   */
  esp_err_t writePin(uint8_t reg, uint8_t val);

  /**
   * @brief Internal function to read a register.
   *
   * @param reg register to read from.
   * @return uint8_t register value.
   *
   * @note Tries to receive indefinitely, until it succeeds.
   */
  uint8_t readPin(uint8_t reg);

  uint16_t m_blockedPins;
  i2c_master_dev_handle_t m_devHandle;
};