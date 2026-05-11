/**
 * @file Inkplate6Color.cpp
 * @author Fran Fodor for Soldered
 * @brief Driver for Inkplate 6 Color board.
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

#include "driver/gpio.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "soc/gpio_sig_map.h"
#include "soc/i2s_struct.h"
#include "string.h"

#include "freertos/FreeRTOS.h"

#include "Inkplate6Color.h"
#include "TPS.h"

// Peripherals defined in BoardCommon.cpp
extern TPS tps;
extern I2C i2c;

static const char *TAG = "INKPLATE6COLOR";

/* -------------------------------------------------------------------------- */
/*                              Public functions                              */
/* -------------------------------------------------------------------------- */

Inkplate6Color::Inkplate6Color()
    : BoardCommon(E_INK_WIDTH, E_INK_HEIGHT, 21, 12),
      m_spi(EPAPER_DIN, EPAPER_CLK) {
  ESP_ERROR_CHECK(initBuffers());

  clearDisplay();

  gpio_set_direction(EPAPER_RST_PIN, GPIO_MODE_OUTPUT);
  gpio_set_direction(EPAPER_DC_PIN, GPIO_MODE_OUTPUT);
  gpio_set_direction(EPAPER_CS_PIN, GPIO_MODE_OUTPUT);
  gpio_set_direction(EPAPER_CLK, GPIO_MODE_OUTPUT);
  gpio_set_direction(EPAPER_DIN, GPIO_MODE_OUTPUT);

  gpio_set_level(EPAPER_RST_PIN, 0);
  gpio_set_level(EPAPER_DC_PIN, 0);
  gpio_set_level(EPAPER_CS_PIN, 0);
  gpio_set_level(EPAPER_CLK, 0);
  gpio_set_level(EPAPER_DIN, 0);

  gpio_set_direction(EPAPER_BUSY_PIN, GPIO_MODE_INPUT);
  gpio_pullup_en(EPAPER_BUSY_PIN);

  // vTaskDelay(pdMS_TO_TICKS(5000));

  if (!setPanelDeepSleep(false))
    ESP_LOGE(TAG, "Panel init failed");

  setPanelDeepSleep(true);
  rtc.begin(i2c.getBusHandle());

  setIOExpanderForLowPower();

  ESP_LOGI(TAG, "Initialization finished!");
}

/**
 * ============================================================
 * Private functions
 * ============================================================
 */

/**
 * @brief  Allocate all framebuffers, DMA buffers, and LUT arrays.
 *
 * @return esp_err_t
 *         ESP_OK on success, ESP_ERR_NO_MEM if any allocation fails
 */
esp_err_t Inkplate6Color::initBuffers() {
  m_framebufferColor = (uint8_t *)heap_caps_malloc(
      E_INK_WIDTH * E_INK_HEIGHT / 2, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  if (!m_framebufferColor)
    return ESP_ERR_NO_MEM;
  memset(m_framebufferColor, 0xFF, E_INK_WIDTH * E_INK_HEIGHT / 2);

  return ESP_OK;
}

/**
 * @brief  Push the 3-bit grayscale framebuffer to the display.
 *
 * @param  bool leaveOn
 *         If true, leaves the e-ink panel powered on after the update
 *
 * @return esp_err_t
 *         ESP_OK on success, or an error code if einkOn() failed
 */
esp_err_t Inkplate6Color::display3b(bool leaveOn) {
  setPanelDeepSleep(false);

  // set resolution setting
  uint8_t data[4] = {0x02, 0x58, 0x01, 0xc0};
  sendCommand(0x61);
  sendData(data, 4);

  // push pixdel data to epaper ram
  sendCommand(0x10);

  sendData(m_framebufferColor, m_einkWidth * m_einkHeight / 2);

  sendCommand(POWER_OFF_REGISTER);
  waitForEpd(60000);
  sendCommand(DISPLAY_REF_REGISTER);
  waitForEpd(60000);
  sendCommand(0x02);
  waitForEpd(60000);

  setPanelDeepSleep(true);

  return ESP_OK;
}

bool Inkplate6Color::waitForEpd(uint32_t timeout) {
  uint32_t elapsed = 0;
  const uint32_t STEP = 10;

  while (gpio_get_level(EPAPER_BUSY_PIN) == 0) {
    if (elapsed >= timeout) {
      ESP_LOGE(TAG, "EPD busy timeout");
      return false;
    }
    vTaskDelay(pdMS_TO_TICKS(STEP));
    elapsed += STEP;
  }
  vTaskDelay(pdMS_TO_TICKS(200));
  return true;
}

void Inkplate6Color::resetPanel() {
  gpio_set_level(EPAPER_RST_PIN, 0);
  vTaskDelay(pdMS_TO_TICKS(1));
  gpio_set_level(EPAPER_RST_PIN, 1);
  vTaskDelay(pdMS_TO_TICKS(200));
}

void Inkplate6Color::sendCommand(uint8_t command) {
  m_spi.sendCommand(command, (gpio_num_t)EPAPER_DC_PIN);
}

void Inkplate6Color::sendData(uint8_t *data, int n) {
  m_spi.sendData(data, n, (gpio_num_t)EPAPER_DC_PIN);
}

void Inkplate6Color::sendData(uint8_t data) {
  m_spi.sendData(data, (gpio_num_t)EPAPER_DC_PIN);
}

bool Inkplate6Color::setPanelDeepSleep(bool sleep) {
  if (!sleep) {
    if (!m_spi.isInitialized())
      m_spi.init();

    // Wake
    gpio_set_direction(EPAPER_BUSY_PIN, GPIO_MODE_INPUT);
    gpio_pullup_en(EPAPER_BUSY_PIN);
    resetPanel();

    waitForEpd(60000);

    uint8_t panel_set_data[] = {0xEF, 0x08};
    sendCommand(PANEL_SET_REGISTER);
    sendData(panel_set_data, 2);

    uint8_t power_set_data[] = {0x37, 0x00, 0x05, 0x05};
    sendCommand(POWER_SET_REGISTER);
    sendData(power_set_data, 4);

    sendCommand(POWER_OFF_SEQ_SET_REGISTER);
    sendData(0x00);

    uint8_t booster_softstart_data[] = {0xC7, 0xC7, 0x1D};
    sendCommand(BOOSTER_SOFTSTART_REGISTER);
    sendData(booster_softstart_data, 3);

    sendCommand(TEMP_SENSOR_EN_REGISTER);
    sendData(0x00);

    sendCommand(VCOM_DATA_INTERVAL_REGISTER);
    sendData(0x37);

    sendCommand(0x60);
    sendData(0x20);

    uint8_t res_set_data[] = {0x02, 0x58, 0x01, 0xC0};
    sendCommand(RESOLUTION_SET_REGISTER);
    sendData(res_set_data, 4);

    sendCommand(0xE3);
    sendData(0xAA);

    vTaskDelay(pdMS_TO_TICKS(100));
    sendCommand(VCOM_DATA_INTERVAL_REGISTER);
    sendData(0x37);

    return true;
  } else {
    // Sleep
    vTaskDelay(pdMS_TO_TICKS(10));
    sendCommand(DEEP_SLEEP_REGISTER);
    sendData(0xA5);

    vTaskDelay(pdMS_TO_TICKS(100));

    gpio_set_level(EPAPER_RST_PIN, 0);
    vTaskDelay(pdMS_TO_TICKS(100));

    gpio_set_level(EPAPER_DC_PIN, 0);
    gpio_set_level(EPAPER_CS_PIN, 0);

    // free SPI bus to release DMA channel for SD card
    m_spi.deinit();

    return true;
  }
}

void Inkplate6Color::setIOExpanderForLowPower() {
  // Battery voltage Switch MOSFET
  expander1.setDirection(IO_NUM_B1, IO_MODE_OUTPUT);
  expander1.setLevel(IO_NUM_B1, 0);

  // Rest of pins go to OUTPUT LOW state because in deepSleep mode they are
  // using least amount of power
  expander1.setDirection(IO_NUM_A0, IO_MODE_OUTPUT);
  expander1.setDirection(IO_NUM_A1, IO_MODE_OUTPUT);
  expander1.setDirection(IO_NUM_A2, IO_MODE_OUTPUT);
  expander1.setDirection(IO_NUM_A3, IO_MODE_OUTPUT);
  expander1.setDirection(IO_NUM_A4, IO_MODE_OUTPUT);
  expander1.setDirection(IO_NUM_A5, IO_MODE_OUTPUT);
  expander1.setDirection(IO_NUM_A6, IO_MODE_OUTPUT);
  expander1.setDirection(IO_NUM_A7, IO_MODE_OUTPUT);
  expander1.setDirection(IO_NUM_B0, IO_MODE_OUTPUT);
  expander1.setDirection(IO_NUM_B5, IO_MODE_OUTPUT);
  expander1.setDirection(IO_NUM_B6, IO_MODE_OUTPUT);
  expander1.setDirection(IO_NUM_B7, IO_MODE_OUTPUT);

  expander1.setLevel(IO_NUM_A0, 0);
  expander1.setLevel(IO_NUM_A1, 0);
  expander1.setLevel(IO_NUM_A2, 0);
  expander1.setLevel(IO_NUM_A3, 0);
  expander1.setLevel(IO_NUM_A4, 0);
  expander1.setLevel(IO_NUM_A5, 0);
  expander1.setLevel(IO_NUM_A6, 0);
  expander1.setLevel(IO_NUM_A7, 0);
  expander1.setLevel(IO_NUM_B0, 0);
  expander1.setLevel(IO_NUM_B5, 0);
  expander1.setLevel(IO_NUM_B6, 0);
  expander1.setLevel(IO_NUM_B7, 0);
}