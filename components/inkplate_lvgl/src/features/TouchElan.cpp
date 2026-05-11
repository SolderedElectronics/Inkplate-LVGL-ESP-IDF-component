/**
 * @file TouchElan.cpp
 * @author Fran Fodor for Soldered
 * @brief Driver for Elan touchscreen.
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

#include "TouchElan.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "TouchElan";

static volatile bool tsFlag = false;

static void IRAM_ATTR tsInt(void * /*arg*/) { tsFlag = true; }

/* -------------------------------------------------------------------------- */
/*                              Public functions                              */
/* -------------------------------------------------------------------------- */

esp_err_t TouchElan::begin(I2C &i2c, PCAL &expander, uint8_t powerState) {
  m_expander = &expander;

  esp_err_t ret = i2c.addDevice(TOUCHSCREEN_I2C_ADDR, &m_devHandle);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to add I2C device: %s", esp_err_to_name(ret));
    return ret;
  }

  m_expander->setLevel(TOUCHSCREEN_EN, 0);
  vTaskDelay(pdMS_TO_TICKS(100));

  gpio_set_direction(TOUCHSCREEN_INT, GPIO_MODE_INPUT);
  gpio_set_intr_type(TOUCHSCREEN_INT, GPIO_INTR_NEGEDGE);

  if (!m_tsInitDone) {
    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    ESP_ERROR_CHECK(gpio_isr_handler_add(TOUCHSCREEN_INT, tsInt, NULL));
  }

  vTaskDelay(pdMS_TO_TICKS(50));

  tsHardwareReset();

  if (!tsSoftwareReset()) {
    ESP_LOGE(TAG, "Touch controller did not respond correctly");
    gpio_isr_handler_remove((gpio_num_t)TOUCHSCREEN_INT);
    return ESP_FAIL;
  }

  tsGetResolution(&m_tsXResolution, &m_tsYResolution);
  ESP_LOGI(TAG, "TS resolution: %u x %u", m_tsXResolution, m_tsYResolution);

  setPowerState(powerState);

  tsFlag = true;
  m_tsInitDone = true;
  return ESP_OK;
}

void TouchElan::shutdown() { end(); }

bool TouchElan::available() { return tsFlag; }

bool TouchElan::touchInArea(int16_t x1, int16_t y1, int16_t w, int16_t h) {
  int16_t x2 = x1 + w;
  int16_t y2 = y1 + h;

  if (available()) {
    uint16_t x[2], y[2];
    uint8_t n = getData(x, y);

    if (n) {
      touchT = (uint32_t)(esp_timer_get_time() / 1000ULL);
      touchN = n;
      memcpy(touchX, x, sizeof(x));
      memcpy(touchY, y, sizeof(y));
    }
  }

  if ((uint32_t)(esp_timer_get_time() / 1000ULL) - touchT < 100) {
    if (touchN == 1 && BOUND(x1, touchX[0], x2) && BOUND(y1, touchY[0], y2))
      return true;

    if (touchN == 2 &&
        ((BOUND(x1, touchX[0], x2) && BOUND(y1, touchY[0], y2)) ||
         (BOUND(x1, touchX[1], x2) && BOUND(y1, touchY[1], y2))))
      return true;
  }

  return false;
}

uint8_t TouchElan::getData(uint16_t *xPos, uint16_t *yPos, uint8_t *z) {
  uint8_t raw[8] = {0};
  uint16_t xRaw[2] = {0, 0};
  uint16_t yRaw[2] = {0, 0};
  uint8_t fingers = 0;

  tsFlag = false;
  getRawData(raw);

  for (int i = 0; i < 8; i++) {
    if (raw[7] & (1 << i))
      fingers++;
  }

  for (int i = 0; i < 2; i++) {
    tsGetXY((raw + 1) + (i * 3), &xRaw[i], &yRaw[i]);

    // Inkplate 4 Tempera: both axes are mirrored relative to the sensor
    switch (m_rotation) {
    case 0:
      yPos[i] =
          E_INK_HEIGHT - 1 - ((xRaw[i] * (E_INK_HEIGHT - 1)) / m_tsXResolution);
      xPos[i] = ((yRaw[i] * (E_INK_WIDTH - 1)) / m_tsYResolution);
      break;
    case 1:
      xPos[i] =
          E_INK_HEIGHT - 1 - ((xRaw[i] * (E_INK_HEIGHT - 1)) / m_tsXResolution);
      yPos[i] =
          E_INK_WIDTH - 1 - ((yRaw[i] * (E_INK_WIDTH - 1)) / m_tsYResolution);
      break;
    case 2:
      yPos[i] = ((xRaw[i] * (E_INK_HEIGHT - 1)) / m_tsXResolution);
      xPos[i] =
          E_INK_WIDTH - 1 - ((yRaw[i] * (E_INK_WIDTH - 1)) / m_tsYResolution);
      break;
    case 3:
      xPos[i] = ((xRaw[i] * (E_INK_HEIGHT - 1)) / m_tsXResolution);
      yPos[i] = ((yRaw[i] * (E_INK_WIDTH - 1)) / m_tsYResolution);
      break;
    }
  }

  return fingers;
}

void TouchElan::getRawData(uint8_t *b) { tsReadRegs(b, 8); }

void TouchElan::setPowerState(uint8_t s) {
  s &= 1;
  uint8_t reg[4] = {0x54, 0x50, 0x00, 0x01};
  reg[1] |= (s << 3);
  tsWriteRegs(reg, 4);
}

uint8_t TouchElan::getPowerState() {
  const uint8_t reg[4] = {0x53, 0x50, 0x00, 0x01};
  uint8_t buf[4] = {0};
  tsWriteRegs(reg, 4);
  tsFlag = false;
  tsReadRegs(buf, 4);
  return (buf[1] >> 3) & 1;
}

/* -------------------------------------------------------------------------- */
/*                              Private functions                             */
/* -------------------------------------------------------------------------- */

uint8_t TouchElan::tsWriteRegs(const uint8_t *buff, uint8_t size) {
  esp_err_t ret = i2c_master_transmit(m_devHandle, buff, size, -1);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "tsWriteRegs failed: %s", esp_err_to_name(ret));
  }
  return (ret == ESP_OK) ? 0 : 1;
}

void TouchElan::tsReadRegs(uint8_t *buff, uint8_t size) {
  esp_err_t ret = i2c_master_receive(m_devHandle, buff, size, -1);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "tsReadRegs failed: %s", esp_err_to_name(ret));
    memset(buff, 0, size);
  }
}

void TouchElan::tsHardwareReset() {
  m_expander->setLevel(TOUCHSCREEN_RST, 0);
  vTaskDelay(pdMS_TO_TICKS(100));
  m_expander->setLevel(TOUCHSCREEN_RST, 1);
  vTaskDelay(pdMS_TO_TICKS(100));
}

bool TouchElan::tsSoftwareReset() {
  const uint8_t softRstCmd[4] = {0x77, 0x77, 0x77, 0x77};

  if (tsWriteRegs(softRstCmd, 4) != 0) {
    ESP_LOGE(TAG, "Software reset write failed");
    return false;
  }

  uint32_t timeout = 1000;
  while (!tsFlag && timeout > 0) {
    vTaskDelay(pdMS_TO_TICKS(1));
    timeout--;
  }

  vTaskDelay(pdMS_TO_TICKS(500));

  if (timeout > 0)
    tsFlag = true;

  uint8_t rb[4] = {0};
  tsReadRegs(rb, 4);
  tsFlag = false;

  if (memcmp(rb, hello_packet, 4) != 0) {
    ESP_LOGE(TAG, "Hello packet mismatch: %02X %02X %02X %02X", rb[0], rb[1],
             rb[2], rb[3]);
    return false;
  }

  return true;
}

void TouchElan::tsGetXY(uint8_t *d, uint16_t *x, uint16_t *y) {
  *x = (uint16_t)(d[0] & 0xF0) << 4;
  *x |= d[1];
  *y = (uint16_t)(d[0] & 0x0F) << 8;
  *y |= d[2];
}

void TouchElan::tsGetResolution(uint16_t *xRes, uint16_t *yRes) {
  const uint8_t cmdX[4] = {0x53, 0x60, 0x00, 0x00};
  const uint8_t cmdY[4] = {0x53, 0x63, 0x00, 0x00};
  uint8_t rec[4] = {0};

  tsWriteRegs(cmdX, 4);
  tsReadRegs(rec, 4);
  *xRes = (uint16_t)(rec[2]) | (uint16_t)((rec[3] & 0xF0) << 4);

  tsWriteRegs(cmdY, 4);
  tsReadRegs(rec, 4);
  *yRes = (uint16_t)(rec[2]) | (uint16_t)((rec[3] & 0xF0) << 4);

  tsFlag = false;
}

void TouchElan::power(bool enable) {
  if (enable) {
    m_expander->setLevel(TOUCHSCREEN_EN, 0);
    vTaskDelay(pdMS_TO_TICKS(50));
    m_expander->setLevel(TOUCHSCREEN_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(50));
  } else {
    m_expander->setLevel(TOUCHSCREEN_EN, 1);
    vTaskDelay(pdMS_TO_TICKS(50));
    m_expander->setLevel(TOUCHSCREEN_RST, 0);
  }
}

void TouchElan::end() {
  if (m_tsInitDone)
    gpio_isr_handler_remove((gpio_num_t)TOUCHSCREEN_INT);

  tsFlag = false;
  power(false);
  m_tsInitDone = false;

  if (m_devHandle) {
    i2c_master_bus_rm_device(m_devHandle);
    m_devHandle = nullptr;
  }
}
