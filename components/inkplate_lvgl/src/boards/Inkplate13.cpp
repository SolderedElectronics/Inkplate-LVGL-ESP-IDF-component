/**
 * @file Inkplate13.cpp
 * @author Fran Fodor for Soldered
 * @brief Driver for Inkplate 13 board.
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

#include "Inkplate13.h"
#include "TPS.h"

// Peripherals defined in BoardCommon.cpp
extern TPS tps;
extern I2C i2c;

static const char *TAG = "INKPLATE13";

/* -------------------------------------------------------------------------- */
/*                              Public functions                              */
/* -------------------------------------------------------------------------- */

Inkplate13::Inkplate13()
    : BoardCommon(E_INK_WIDTH, E_INK_HEIGHT, 0, 0),
      m_spi(SPECTRA133_SPI_MOSI, SPECTRA133_SPI_SCK, (gpio_num_t)-1,
            SPI3_HOST) {
  ESP_ERROR_CHECK(initBuffers());

  clearDisplay();

  gpio_set_direction(SPECTRA133_RST_PIN, GPIO_MODE_OUTPUT);
  gpio_set_direction(SPECTRA133_DC_PIN, GPIO_MODE_OUTPUT);
  gpio_set_direction(SPECTRA133_CS_M_PIN, GPIO_MODE_OUTPUT);
  gpio_set_direction(SPECTRA133_CS_S_PIN, GPIO_MODE_OUTPUT);
  gpio_set_direction(SPECTRA133_PWR_EN, GPIO_MODE_OUTPUT);
  gpio_set_direction(SPECTRA133_BS0, GPIO_MODE_OUTPUT);
  gpio_set_direction(SPECTRA133_BS1, GPIO_MODE_OUTPUT);

  gpio_set_level(SPECTRA133_RST_PIN, 0);
  gpio_set_level(SPECTRA133_DC_PIN, 0);
  gpio_set_level(SPECTRA133_CS_M_PIN, 0);
  gpio_set_level(SPECTRA133_CS_S_PIN, 0);
  gpio_set_level(SPECTRA133_PWR_EN, 0);
  gpio_set_level(SPECTRA133_BS0, 0);
  gpio_set_level(SPECTRA133_BS1, 0);

  gpio_set_direction(SPECTRA133_BUSYN_PIN, GPIO_MODE_INPUT);
  gpio_pullup_en(SPECTRA133_BUSYN_PIN);

  if (!setPanelDeepSleep(false))
    ESP_LOGE(TAG, "Panel init failed");

  setPanelDeepSleep(true);

  rtc.begin(i2c.getBusHandle());

  ESP_LOGI(TAG, "Initialization finished!");
}

/* -------------------------------------------------------------------------- */
/*                              Private functions                             */
/* -------------------------------------------------------------------------- */

esp_err_t Inkplate13::initBuffers() {
  m_framebufferColor = (uint8_t *)heap_caps_malloc(
      E_INK_WIDTH * E_INK_HEIGHT / 2, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  if (!m_framebufferColor)
    return ESP_ERR_NO_MEM;

  return ESP_OK;
}

esp_err_t Inkplate13::display3b(bool leaveOn) {
  setPanelDeepSleep(false);

  gpio_set_level(SPECTRA133_CS_M_PIN, 0);
  gpio_set_level(SPECTRA133_CS_S_PIN, 1);
  m_spi.sendCommand(SPECTRA133_REGISTER_DTM, SPECTRA133_DC_PIN);
  for (int i = 0; i < E_INK_HEIGHT; i++)
    m_spi.sendData(m_framebufferColor + (i * E_INK_WIDTH / 2), E_INK_WIDTH / 4,
                   SPECTRA133_DC_PIN);

  gpio_set_level(SPECTRA133_CS_M_PIN, 1);

  waitForEpd(60000);

  gpio_set_level(SPECTRA133_CS_S_PIN, 0);
  gpio_set_level(SPECTRA133_CS_M_PIN, 1);

  m_spi.sendCommand(SPECTRA133_REGISTER_DTM, SPECTRA133_DC_PIN);
  for (int i = 0; i < E_INK_HEIGHT; i++)
    m_spi.sendData(m_framebufferColor + (i * E_INK_WIDTH / 2) +
                       (E_INK_WIDTH / 4),
                   E_INK_WIDTH / 4, SPECTRA133_DC_PIN);

  gpio_set_level(SPECTRA133_CS_S_PIN, 1);
  waitForEpd(60000);

  sendCommandData(SPECTRA133_REGISTER_DRF, SPECTRA133_REGISTER_DRF_V,
                  sizeof(SPECTRA133_REGISTER_DRF_V), eChipIdBoth);
  waitForEpd(60000);

  if (!leaveOn)
    setPanelDeepSleep(true);

  return ESP_OK;
}

bool Inkplate13::waitForEpd(uint32_t timeout) {
  uint32_t elapsed = 0;
  const uint32_t STEP = 10;

  while (gpio_get_level(SPECTRA133_BUSYN_PIN) == 0) {
    if (elapsed >= timeout) {
      return false;
    }
    vTaskDelay(pdMS_TO_TICKS(STEP));
    elapsed += STEP;
  }

  return true;
}

void Inkplate13::resetPanel() {
  gpio_set_level(SPECTRA133_RST_PIN, 0);
  vTaskDelay(pdMS_TO_TICKS(100));
  gpio_set_level(SPECTRA133_RST_PIN, 1);
  vTaskDelay(pdMS_TO_TICKS(100));
}

void Inkplate13::sendCommandData(uint8_t cmd, uint8_t *data, int n,
                                 enum eSpectraChipID chipId) {
  if (chipId & eChipIdMaster)
    gpio_set_level(SPECTRA133_CS_M_PIN, 0);
  if (chipId & eChipIdSlave)
    gpio_set_level(SPECTRA133_CS_S_PIN, 0);

  m_spi.sendCommand(cmd, SPECTRA133_DC_PIN);

  if (data && n > 0)
    m_spi.sendData(data, n, SPECTRA133_DC_PIN);

  if (chipId & eChipIdMaster)
    gpio_set_level(SPECTRA133_CS_M_PIN, 1);
  if (chipId & eChipIdSlave)
    gpio_set_level(SPECTRA133_CS_S_PIN, 1);
}

bool Inkplate13::setPanelDeepSleep(bool sleep) {
  if (!sleep) {
    if (!m_spi.isInitialized())
      m_spi.init();

    setPanelPinsToLow();
    vTaskDelay(pdMS_TO_TICKS(50));

    gpio_set_level(SPECTRA133_DC_PIN, 1);
    gpio_set_level(SPECTRA133_CS_M_PIN, 1);
    gpio_set_level(SPECTRA133_CS_S_PIN, 1);
    gpio_set_level(SPECTRA133_RST_PIN, 0);
    gpio_set_level(SPECTRA133_BS0, 0);
    gpio_set_level(SPECTRA133_BS1, 1);

    gpio_set_level(SPECTRA133_PWR_EN, 1);
    vTaskDelay(pdMS_TO_TICKS(100));

    resetPanel();
    vTaskDelay(pdMS_TO_TICKS(100));

    screenInit();

    sendCommandData(SPECTRA133_REGISTER_PON, nullptr, 0, eChipIdBoth);
    waitForEpd(60000);
    return true;
  } else {
    sendCommandData(SPECTRA133_REGISTER_POF, SPECTRA133_REGISTER_POF_V,
                    sizeof(SPECTRA133_REGISTER_POF_V), eChipIdBoth);
    waitForEpd(60000);

    gpio_set_direction(SPECTRA133_DC_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(SPECTRA133_CS_M_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(SPECTRA133_CS_S_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(SPECTRA133_RST_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(SPECTRA133_BUSYN_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(SPECTRA133_PWR_EN, GPIO_MODE_INPUT);

    gpio_set_level(SPECTRA133_PWR_EN, 0);

    m_spi.deinit();

    return true;
  }
}

void Inkplate13::setPanelPinsToLow() {
  const gpio_num_t pins[] = {SPECTRA133_DC_PIN,   SPECTRA133_CS_M_PIN,
                             SPECTRA133_CS_S_PIN, SPECTRA133_RST_PIN,
                             SPECTRA133_PWR_EN,   SPECTRA133_BS0,
                             SPECTRA133_BS1};
  for (auto p : pins) {
    gpio_set_direction(p, GPIO_MODE_OUTPUT);
    gpio_set_level(p, 0);
  }
}

void Inkplate13::screenInit() {
  sendCommandData(SPECTRA133_REGISTER_AN_TM, SPECTRA133_REGISTER_AN_TM_V,
                  sizeof(SPECTRA133_REGISTER_AN_TM_V), eChipIdMaster);

  sendCommandData(SPECTRA133_REGISTER_CMD66, SPECTRA133_REGISTER_CMD66_V,
                  sizeof(SPECTRA133_REGISTER_CMD66_V), eChipIdBoth);
  sendCommandData(SPECTRA133_REGISTER_PSR, SPECTRA133_REGISTER_PSR_V,
                  sizeof(SPECTRA133_REGISTER_PSR_V), eChipIdBoth);
  sendCommandData(SPECTRA133_REGISTER_PLL, SPECTRA133_REGISTER_PLL_V,
                  sizeof(SPECTRA133_REGISTER_PLL_V), eChipIdBoth);
  sendCommandData(SPECTRA133_REGISTER_CDI, SPECTRA133_REGISTER_CDI_V,
                  sizeof(SPECTRA133_REGISTER_CDI_V), eChipIdBoth);
  sendCommandData(SPECTRA133_REGISTER_TCON, SPECTRA133_REGISTER_TCON_V,
                  sizeof(SPECTRA133_REGISTER_TCON_V), eChipIdBoth);
  sendCommandData(SPECTRA133_REGISTER_AGID, SPECTRA133_REGISTER_AGID_V,
                  sizeof(SPECTRA133_REGISTER_AGID_V), eChipIdBoth);
  sendCommandData(SPECTRA133_REGISTER_PWS, SPECTRA133_REGISTER_PWS_V,
                  sizeof(SPECTRA133_REGISTER_PWS_V), eChipIdBoth);
  sendCommandData(SPECTRA133_REGISTER_CCSET, SPECTRA133_REGISTER_CCSET_V,
                  sizeof(SPECTRA133_REGISTER_CCSET_V), eChipIdBoth);
  sendCommandData(SPECTRA133_REGISTER_TRES, SPECTRA133_REGISTER_TRES_V,
                  sizeof(SPECTRA133_REGISTER_TRES_V), eChipIdBoth);

  sendCommandData(SPECTRA133_REGISTER_PWR, SPECTRA133_REGISTER_PWR_V,
                  sizeof(SPECTRA133_REGISTER_PWR_V), eChipIdMaster);
  sendCommandData(SPECTRA133_REGISTER_EN_BUF, SPECTRA133_REGISTER_EN_BUF_V,
                  sizeof(SPECTRA133_REGISTER_EN_BUF_V), eChipIdMaster);
  sendCommandData(SPECTRA133_REGISTER_BTST_P, SPECTRA133_REGISTER_BTST_P_V,
                  sizeof(SPECTRA133_REGISTER_BTST_P_V), eChipIdMaster);
  sendCommandData(SPECTRA133_REGISTER_BOOST_VDDP_EN,
                  SPECTRA133_REGISTER_BOOST_VDDP_EN_V,
                  sizeof(SPECTRA133_REGISTER_BOOST_VDDP_EN_V), eChipIdMaster);
  sendCommandData(SPECTRA133_REGISTER_BTST_N, SPECTRA133_REGISTER_BTST_N_V,
                  sizeof(SPECTRA133_REGISTER_BTST_N_V), eChipIdMaster);
  sendCommandData(SPECTRA133_REGISTER_BUCK_BOOST_VDDN,
                  SPECTRA133_REGISTER_BUCK_BOOST_VDDN_V,
                  sizeof(SPECTRA133_REGISTER_BUCK_BOOST_VDDN_V), eChipIdMaster);
  sendCommandData(SPECTRA133_REGISTER_TFT_VCOM_POWER,
                  SPECTRA133_REGISTER_TFT_VCOM_POWER_V,
                  sizeof(SPECTRA133_REGISTER_TFT_VCOM_POWER_V), eChipIdMaster);
}
