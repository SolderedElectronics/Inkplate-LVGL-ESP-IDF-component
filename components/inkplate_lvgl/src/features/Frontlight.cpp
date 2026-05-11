/**
 * @file Frontlight.cpp
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

#include "esp_log.h"

#include "Frontlight.h"

static const char *TAG = "FRONTLIGHT";

/* -------------------------------------------------------------------------- */
/*                              Public functions                              */
/* -------------------------------------------------------------------------- */

esp_err_t Frontlight::begin(I2C &i2c, PCAL &expander, IOPin_t pin) {
  m_expander = &expander;
  m_pin = pin;
  m_expander->setDirection(m_pin, IO_MODE_OUTPUT);
  return i2c.addDevice(FRONTLIGHT_I2C_ADDR, &m_devHandle);
}

esp_err_t Frontlight::setBrightness(uint8_t value) {
  ESP_LOGI(TAG, "Set brightness %d", value);
  uint8_t buf[2] = {0, (uint8_t)(63 - (value & 0x3F))};
  return i2c_master_transmit(m_devHandle, buf, sizeof(buf), -1);
}

void Frontlight::setState(bool enable) {
  ESP_LOGI(TAG, "Set state %d", enable);
  m_expander->setLevel(m_pin, enable ? 1 : 0, true);
}
