/**
 * @file PCAL.cpp
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

#include "driver/i2c_master.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

#include "PCAL.h"

static const char *TAG = "ESP_PCAL";

/* -------------------------------------------------------------------------- */
/*                              Public functions                              */
/* -------------------------------------------------------------------------- */

PCAL::PCAL(uint8_t addr, I2C &i2c) {
  ESP_ERROR_CHECK(i2c.addDevice(addr, &m_devHandle));

  m_blockedPins = 0;

  ESP_LOGI(TAG, "I2C initilization finished!");
}

esp_err_t PCAL::setLevel(IOPin_t pin, uint8_t level, bool bypass) {
  if (!bypass && checkBlockedPins(pin))
    return ESP_ERR_INVALID_ARG;

  // check if pin is set as input
  uint8_t cfgReg, cfgBit;
  pinToRegBit(pin, PCAL6416A_CFGPORT0, cfgReg, cfgBit);
  if ((readPin(cfgReg) >> cfgBit) & 1)
    return ESP_ERR_INVALID_STATE;

  uint8_t reg, bit;
  pinToRegBit(pin, PCAL6416A_OUTPORT0, reg, bit);

  uint8_t val = readPin(reg);
  if (level)
    val |= (1 << bit);
  else
    val &= ~(1 << bit);

  return writePin(reg, val);
}

int PCAL::getLevel(IOPin_t pin, bool bypass) {
  if (!bypass && checkBlockedPins(pin))
    return ESP_ERR_INVALID_ARG;

  uint8_t reg, bit;
  pinToRegBit(pin, PCAL6416A_INPORT0, reg, bit);

  return (readPin(reg) >> bit) & 1;
}

esp_err_t PCAL::setPort(IOPort_t port, uint8_t value) {
  uint8_t reg = (port == IO_PORT_0) ? PCAL6416A_OUTPORT0 : PCAL6416A_OUTPORT1;
  return writePin(reg, value);
}

int PCAL::getPort(IOPort_t port) {
  uint8_t reg = (port == IO_PORT_0) ? PCAL6416A_INPORT0 : PCAL6416A_INPORT1;
  return readPin(reg);
}

esp_err_t PCAL::setPortDirection(IOPort_t port, uint8_t mask) {
  uint8_t reg = (port == IO_PORT_0) ? PCAL6416A_CFGPORT0 : PCAL6416A_CFGPORT1;
  return writePin(reg, mask);
}

esp_err_t PCAL::setDirection(IOPin_t pin, IOMode_t mode, bool bypass) {
  if (!bypass && checkBlockedPins(pin))
    return ESP_ERR_INVALID_ARG;

  uint8_t reg, bit;
  pinToRegBit(pin, PCAL6416A_CFGPORT0, reg, bit);

  // 1 = input, 0 = output
  uint8_t val = readPin(reg);
  if (mode == IO_MODE_INPUT)
    val |= (1 << bit);
  else
    val &= ~(1 << bit);

  return writePin(reg, val);
}

esp_err_t PCAL::setPullMode(IOPin_t pin, IOPullMode_t pullMode, bool bypass) {
  if (!bypass && checkBlockedPins(pin))
    return ESP_ERR_INVALID_ARG;

  uint8_t enReg, selReg, bit;
  pinToRegBit(pin, PCAL6416A_PUPDEN_REG0, enReg, bit);
  pinToRegBit(pin, PCAL6416A_PUPDSEL_REG0, selReg, bit);

  // enable the pull resistor for this pin
  uint8_t enVal = readPin(enReg);
  enVal |= (1 << bit);
  esp_err_t ret = writePin(enReg, enVal);
  if (ret != ESP_OK)
    return ret;

  // select pull-up (1) or pull-down (0)
  uint8_t selVal = readPin(selReg);
  if (pullMode == IO_PULLUP)
    selVal |= (1 << bit);
  else
    selVal &= ~(1 << bit);

  return writePin(selReg, selVal);
}

esp_err_t PCAL::setPolarityInversion(IOPin_t pin, bool invert, bool bypass) {
  if (!bypass && checkBlockedPins(pin))
    return ESP_ERR_INVALID_ARG;

  uint8_t reg, bit;
  pinToRegBit(pin, PCAL6416A_POLINVPORT0, reg, bit);

  uint8_t val = readPin(reg);
  if (invert)
    val |= (1 << bit);
  else
    val &= ~(1 << bit);

  return writePin(reg, val);
}

esp_err_t PCAL::setInputLatch(IOPin_t pin, bool latch, bool bypass) {
  if (!bypass && checkBlockedPins(pin))
    return ESP_ERR_INVALID_ARG;

  uint8_t reg, bit;
  pinToRegBit(pin, PCAL6416A_INLAT_REG0, reg, bit);

  uint8_t val = readPin(reg);
  if (latch)
    val |= (1 << bit);
  else
    val &= ~(1 << bit);

  return writePin(reg, val);
}

esp_err_t PCAL::setOutputMode(IOPort_t port, IOOutputMode_t mode) {
  uint8_t bit = (port == IO_PORT_0) ? 0 : 1;

  uint8_t val = readPin(PCAL6416A_OUTPORT_CONF);
  if (mode == IO_OPEN_DRAIN)
    val |= (1 << bit);
  else
    val &= ~(1 << bit);

  return writePin(PCAL6416A_OUTPORT_CONF, val);
}

esp_err_t PCAL::setDriveStrength(IOPin_t pin, IODriveStrength_t strength,
                                 bool bypass) {
  if (!bypass && checkBlockedPins(pin))
    return ESP_ERR_INVALID_ARG;

  // two registers per port (pins 0-3 in first, pins 4-7 in second)
  uint8_t pinIndex = pin % 8; // pin index within its port
  uint8_t baseReg =
      (pin >= IO_NUM_B0) ? PCAL6416A_OUTDRVST_REG10 : PCAL6416A_OUTDRVST_REG00;
  uint8_t reg = baseReg + (pinIndex / 4);
  uint8_t shift = (pinIndex % 4) * 2;

  uint8_t val = readPin(reg);
  val &= ~(0x03 << shift);
  val |= ((uint8_t)strength << shift);

  return writePin(reg, val);
}

esp_err_t PCAL::interruptEnable(IOPin_t pin, bool bypass) {
  if (!bypass && checkBlockedPins(pin))
    return ESP_ERR_INVALID_ARG;

  uint8_t reg, bit;
  pinToRegBit(pin, PCAL6416A_INTMSK_REG0, reg, bit);

  // 0 = interrupt enabled, 1 = masked
  uint8_t val = readPin(reg);
  val &= ~(1 << bit);

  return writePin(reg, val);
}

esp_err_t PCAL::interruptDisable(IOPin_t pin, bool bypass) {
  if (!bypass && checkBlockedPins(pin))
    return ESP_ERR_INVALID_ARG;

  uint8_t reg, bit;
  pinToRegBit(pin, PCAL6416A_INTMSK_REG0, reg, bit);

  // 1 = masked (disabled)
  uint8_t val = readPin(reg);
  val |= (1 << bit);

  return writePin(reg, val);
}

bool PCAL::getInterrupt(IOPin_t pin) {
  uint8_t reg, bit;
  pinToRegBit(pin, PCAL6416A_INTSTAT_REG0, reg, bit);

  return (readPin(reg) >> bit) & 1;
}

esp_err_t PCAL::blockPin(IOPin_t pin) {
  m_blockedPins |= (1 << pin);
  return ESP_OK;
}

esp_err_t PCAL::unblockPin(IOPin_t pin) {
  m_blockedPins &= ~(1 << pin);
  return ESP_OK;
}

/* -------------------------------------------------------------------------- */
/*                              Private functions                             */
/* -------------------------------------------------------------------------- */

void PCAL::pinToRegBit(IOPin_t pin, uint8_t baseReg, uint8_t &reg,
                       uint8_t &bit) {
  reg = baseReg + (pin >= IO_NUM_B0 ? 1 : 0);
  bit = pin % 8;
}

bool PCAL::checkBlockedPins(IOPin_t pin) { return (m_blockedPins >> pin) & 1; }

esp_err_t PCAL::writePin(uint8_t reg, uint8_t val) {
  uint8_t data[2] = {reg, val};
  while (i2c_master_transmit(m_devHandle, data, sizeof(data), 50) != ESP_OK) {
    ESP_LOGI(TAG, "Waiting on I2C bus...");
    vTaskDelay(pdMS_TO_TICKS(300));
  }

  return ESP_OK;
}

uint8_t PCAL::readPin(uint8_t reg) {
  uint8_t val = 0;
  while (i2c_master_transmit_receive(m_devHandle, &reg, 1, &val, 1, 50) !=
         ESP_OK) {
    ESP_LOGI(TAG, "Waiting on I2C bus...");
    vTaskDelay(pdMS_TO_TICKS(300));
  }

  return val;
}
