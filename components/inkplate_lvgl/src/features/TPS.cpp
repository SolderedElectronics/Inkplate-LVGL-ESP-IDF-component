/**
 * @file TPS.cpp
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

#include "esp_log.h"
#include "esp_rom_sys.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"

#include "TPS.h"

static const char *TAG = "TPS";

/* -------------------------------------------------------------------------- */
/*                              Public functions                              */
/* -------------------------------------------------------------------------- */

TPS::TPS(I2C &i2c) { ESP_ERROR_CHECK(i2c.addDevice(TPS_I2C_ADDR, &m_handle)); }

esp_err_t TPS::initSequences() {
  uint8_t buf[5] = {0x09, 0b00011011, 0b00000000, 0b00011011, 0b00000000};
  return i2c_master_transmit(m_handle, buf, sizeof(buf), -1);
}

esp_err_t TPS::enableRails() { return writeReg(0x01, 0b00100000); }

esp_err_t TPS::disableRails() { return writeReg(0x01, 0b00000000); }

esp_err_t TPS::setPowerUpSequence(uint8_t seq) { return writeReg(0x09, seq); }

esp_err_t TPS::setPowerDownSequence(uint8_t seq) { return writeReg(0x0B, seq); }

uint8_t TPS::readPowerGood() { return readReg(0x0F); }

bool TPS::waitPowerGood(bool target) {
  int64_t timer = esp_timer_get_time();
  do {
    esp_rom_delay_us(1000);
  } while ((readPowerGood() == TPS_PWR_GOOD) != target &&
           (esp_timer_get_time() - timer) < 250000LL);

  return (esp_timer_get_time() - timer) < 250000LL;
}

esp_err_t TPS::writeVCOM(double vcom, PCAL &expander) {
  if (vcom < -5.0 || vcom > 0.0)
    return ESP_ERR_INVALID_ARG;

  // Configure INT pin as input with pull-up
  expander.setDirection(TPS_INT_PIN, IO_MODE_INPUT, true);
  expander.setPullMode(TPS_INT_PIN, IO_PULLUP, true);

  // Convert to 9-bit raw value (abs * 100, e.g. -1.23V → 123)
  int raw = abs((int)(vcom * 100.0)) & 0x1FF;
  uint8_t vcomL = (uint8_t)(raw & 0xFF);
  uint8_t vcomMSB = (uint8_t)((raw >> 8) & 0x01);

  // Write low 8 bits to REG_VCOM1 (0x03)
  writeReg(0x03, vcomL);

  // Read REG_VCOM2 (0x04), preserve reserved bits, set MSB, clear program bit
  uint8_t r4 = readReg(0x04);
  r4 &= ~((1 << 0) | (1 << 6));
  r4 |= vcomMSB;
  writeReg(0x04, r4);
  esp_rom_delay_us(1000);

  // Strobe EEPROM program bit (bit 6)
  writeReg(0x04, r4 | (1 << 6));

  // Wait for INT to go low (programming done), 1 s timeout
  int64_t deadline = esp_timer_get_time() + 1000000LL;
  while (expander.getLevel(TPS_INT_PIN, true) &&
         esp_timer_get_time() < deadline)
    esp_rom_delay_us(1000);

  // Clear interrupt by reading INT1 register (0x07)
  (void)readReg(0x07);

  // Readback verification
  uint8_t rdL = readReg(0x03);
  uint8_t rdH = readReg(0x04) & 0x01;
  int check = ((int)rdH << 8) | rdL;

  ESP_LOGI(TAG, "VCOM program: raw=%d readback=%d %s", raw, check,
           (check == raw) ? "OK" : "FAIL");

  return (check == raw) ? ESP_OK : ESP_FAIL;
}

double TPS::readVCOM() {
  uint8_t vcomL = readReg(0x03);
  uint8_t vcomH = readReg(0x04) & 0x01;
  int raw = ((int)vcomH << 8) | vcomL;
  return -(raw / 100.0);
}

int8_t TPS::readTemperature() {
  writeReg(0x0D, 0x80);
  vTaskDelay(pdMS_TO_TICKS(50));
  uint8_t raw = readReg(0x00);
  return (int8_t)raw;
}

/* -------------------------------------------------------------------------- */
/*                              Private functions                             */
/* -------------------------------------------------------------------------- */

esp_err_t TPS::writeReg(uint8_t reg, uint8_t val) {
  uint8_t buf[2] = {reg, val};
  return i2c_master_transmit(m_handle, buf, sizeof(buf), -1);
}

uint8_t TPS::readReg(uint8_t reg) {
  uint8_t val = 0;
  i2c_master_transmit_receive(m_handle, &reg, 1, &val, 1, -1);
  return val;
}
