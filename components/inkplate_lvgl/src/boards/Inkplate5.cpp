/**
 * @file Inkplate5.cpp
 * @author Fran Fodor for Soldered
 * @brief Driver for Inkplate 5 board.
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

#include "I2C.h"
#include "Inkplate5.h"
#include "TPS.h"

// Peripherals defined in BoardCommon.cpp
extern TPS tps;
extern I2C i2c;

static const char *TAG = "INKPLATE5";

/* -------------------------------------------------------------------------- */
/*                              Public functions                              */
/* -------------------------------------------------------------------------- */

Inkplate5::Inkplate5() : BoardCommon(E_INK_WIDTH, E_INK_HEIGHT, 21, 12) {
  ESP_ERROR_CHECK(initBuffers());
  calculateLUTs();
  gpioInit();
  blockGpioPins();
  ESP_ERROR_CHECK(pmicBegin());
  rtc.begin(i2c.getBusHandle());

  ESP_LOGI(TAG, "Initialization finished!");
}

uint32_t Inkplate5::partialUpdate(bool forced, bool leaveOn) {
  if (m_displayMode == GRAYSCALE) {
    ESP_LOGI(TAG, "Selected display mode does not support partial updating.");
    return 0;
  }

  if (m_blockPartial && !forced) {
    display1b(leaveOn);
    return 0;
  }

  if (m_partialUpdateCounter >= m_partialUpdateLimiter &&
      m_partialUpdateLimiter != 0) {
    ESP_LOGI(TAG, "Partial update limit reached, forcing full update.");
    display1b(leaveOn);
    m_partialUpdateCounter = 0;
    return 0;
  }

  uint16_t position = (m_einkWidth * m_einkHeight / 8) - 1;
  uint32_t n = (m_einkWidth * m_einkHeight / 4) - 1;
  uint8_t diffWhite, diffBlack;
  uint32_t changeCount = 0;

  m_dmaI2SDesc->size = (m_einkWidth / 4) + 16;
  m_dmaI2SDesc->length = (m_einkWidth / 4) + 16;
  m_dmaI2SDesc->sosf = 1;
  m_dmaI2SDesc->owner = 1;
  m_dmaI2SDesc->qe.stqe_next = 0;
  m_dmaI2SDesc->eof = 1;
  m_dmaI2SDesc->buf = m_dmaLineBuffer;
  m_dmaI2SDesc->offset = 0;

  for (int i = 0; i < m_einkHeight; i++) {
    for (int j = 0; j < m_einkWidth / 8; j++) {
      diffWhite = *(m_framebuffer + position) & ~*(m_newFramebuffer + position);
      diffBlack = ~*(m_framebuffer + position) & *(m_newFramebuffer + position);
      if (diffWhite) {
        for (int bv = 1; bv < 256; bv <<= 1)
          if (diffWhite & bv)
            changeCount++;
      }
      position--;
      *(m_waveformBuffer + n) = LUTW[diffWhite >> 4] & LUTB[diffBlack >> 4];
      n--;
      *(m_waveformBuffer + n) = LUTW[diffWhite & 0x0F] & LUTB[diffBlack & 0x0F];
      n--;
    }
  }

  if (einkOn() != ESP_OK)
    return 0;

  uint8_t rep = 4;
  for (int k = 0; k < rep; k++) {
    uint8_t *dp = m_waveformBuffer;
    vscanStart();

    for (int i = 0; i < m_einkHeight; i++) {
      uint8_t *dpFlipped = (dp + E_INK_WIDTH / 4) - 1;
      for (int j = 0; j < (m_einkWidth / 4); j += 4) {
        m_dmaLineBuffer[j + 2] = *(dpFlipped);
        m_dmaLineBuffer[j + 3] = *(dpFlipped);
        m_dmaLineBuffer[j] = *(dpFlipped);
        m_dmaLineBuffer[j + 1] = *(dpFlipped);
      }
      dp += (E_INK_WIDTH / 4);
      sendDataI2S();
      vscanEnd();
    }
    esp_rom_delay_us(230);
  }

  clean(2, 2);
  // vscanStart();

  if (einkOn() != ESP_OK)
    einkOff();

  memcpy(m_framebuffer, m_newFramebuffer, m_einkWidth * m_einkHeight / 8);

  if (m_partialUpdateLimiter != 0)
    m_partialUpdateCounter++;

  return changeCount;
}

esp_err_t Inkplate5::einkOn() {
  if (getPanelState())
    return ESP_OK;

  WAKEUP_SET;
  esp_rom_delay_us(5000);

  tps.enableRails();
  tps.setPowerUpSequence(TPS_PWRUP_SEQ);
  tps.setPowerDownSequence(TPS_PWRDN_SEQ);

  pinsAsOutputs();
  LE_CLEAR;

  SPH_SET;
  GMOD_SET;
  SPV_SET;
  CKV_CLEAR;
  OE_CLEAR;
  PWRUP_SET;
  setPanelState(true);

  if (!tps.waitPowerGood(true)) {
    einkOff();
    return ESP_ERR_TIMEOUT;
  }

  ESP_LOGI(TAG, "Eink turned on.");

  VCOM_SET;
  OE_SET;
  return ESP_OK;
}

esp_err_t Inkplate5::einkOff() {
  if (!getPanelState())
    return ESP_OK;

  VCOM_CLEAR;
  OE_CLEAR;
  GMOD_CLEAR;
  LE_CLEAR;

  CKV_CLEAR;
  SPH_CLEAR;
  SPV_CLEAR;
  PWRUP_CLEAR;

  tps.waitPowerGood(false);

  WAKEUP_CLEAR;
  esp_err_t ret = tps.disableRails();

  pinsZstate();
  setPanelState(false);

  ESP_LOGI(TAG, "Eink turned off.");
  return ret;
}

/* -------------------------------------------------------------------------- */
/*                              Private functions                             */
/* -------------------------------------------------------------------------- */

esp_err_t Inkplate5::initBuffers() {
  m_framebufferColor = (uint8_t *)heap_caps_malloc(
      E_INK_WIDTH * E_INK_HEIGHT / 2, MALLOC_CAP_SPIRAM);
  if (!m_framebufferColor)
    return ESP_ERR_NO_MEM;
  memset(m_framebufferColor, 0xFF, E_INK_WIDTH * E_INK_HEIGHT / 2);

  m_framebuffer = (uint8_t *)heap_caps_malloc(E_INK_WIDTH * E_INK_HEIGHT / 8,
                                              MALLOC_CAP_SPIRAM);
  if (!m_framebuffer)
    return ESP_ERR_NO_MEM;
  memset(m_framebuffer, 0x00, E_INK_WIDTH * E_INK_HEIGHT / 8);

  m_newFramebuffer = (uint8_t *)heap_caps_malloc(E_INK_WIDTH * E_INK_HEIGHT / 8,
                                                 MALLOC_CAP_SPIRAM);
  if (!m_newFramebuffer)
    return ESP_ERR_NO_MEM;
  memset(m_newFramebuffer, 0x00, E_INK_WIDTH * E_INK_HEIGHT / 8);

  m_waveformBuffer = (uint8_t *)heap_caps_malloc(E_INK_WIDTH * E_INK_HEIGHT / 4,
                                                 MALLOC_CAP_SPIRAM);
  if (!m_waveformBuffer)
    return ESP_ERR_NO_MEM;
  memset(m_waveformBuffer, 0x00, E_INK_WIDTH * E_INK_HEIGHT / 4);

  m_dmaLineBuffer = (volatile uint8_t *)heap_caps_malloc((E_INK_WIDTH / 4) + 16,
                                                         MALLOC_CAP_DMA);
  if (!m_dmaLineBuffer)
    return ESP_ERR_NO_MEM;

  m_dmaI2SDesc =
      (volatile lldesc_s *)heap_caps_malloc(sizeof(lldesc_s), MALLOC_CAP_DMA);
  if (!m_dmaI2SDesc)
    return ESP_ERR_NO_MEM;

  m_glut = (uint32_t *)heap_caps_malloc(9 * 256 * sizeof(uint32_t),
                                        MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
  if (!m_glut)
    return ESP_ERR_NO_MEM;

  m_glut2 = (uint32_t *)heap_caps_malloc(9 * 256 * sizeof(uint32_t),
                                         MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
  if (!m_glut2)
    return ESP_ERR_NO_MEM;

  m_pinLUT = (uint32_t *)heap_caps_malloc(
      256 * sizeof(uint32_t), MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
  if (!m_pinLUT)
    return ESP_ERR_NO_MEM;

  return ESP_OK;
}

void Inkplate5::calculateLUTs() {
  for (int j = 0; j < 9; ++j)
    for (int i = 0; i < 256; ++i) {
      m_glut[j * 256 + i] =
          (waveform3Bit[i & 0x07][j] << 2) | (waveform3Bit[(i >> 4) & 0x07][j]);
      m_glut2[j * 256 + i] = ((waveform3Bit[i & 0x07][j] << 2) |
                              (waveform3Bit[(i >> 4) & 0x07][j]))
                             << 4;
    }
}

esp_err_t Inkplate5::display3b(bool leaveOn) {
  esp_err_t ret = einkOn();
  if (ret != ESP_OK) {
    ESP_LOGI(TAG, "Display is not on!");
    return ret;
  }

  clean(0, 1);
  clean(1, 11);
  clean(2, 1);
  clean(0, 11);
  clean(2, 1);
  clean(1, 11);
  clean(2, 1);
  clean(0, 11);

  for (int k = 0; k < 9; ++k) {
    uint8_t *dp = m_framebufferColor;
    vscanStart();

    for (int i = 0; i < E_INK_HEIGHT; ++i) {
      uint8_t *framebufferFlipped = (dp + E_INK_WIDTH / 2) - 1;

      for (int j = 0; j < (E_INK_WIDTH / 4); j += 4) {
        uint8_t p0, p1;
        p0 = *(--framebufferFlipped);
        p1 = *(--framebufferFlipped);
        m_dmaLineBuffer[j + 2] = (m_glut2[k * 256 + p0] | m_glut[k * 256 + p1]);
        p0 = *(--framebufferFlipped);
        p1 = *(--framebufferFlipped);
        m_dmaLineBuffer[j + 3] = (m_glut2[k * 256 + p0] | m_glut[k * 256 + p1]);
        p0 = *(--framebufferFlipped);
        p1 = *(--framebufferFlipped);
        m_dmaLineBuffer[j] = (m_glut2[k * 256 + p0] | m_glut[k * 256 + p1]);
        p0 = *(--framebufferFlipped);
        p1 = *(--framebufferFlipped);
        m_dmaLineBuffer[j + 1] = (m_glut2[k * 256 + p0] | m_glut[k * 256 + p1]);
        dp += 8;
      }
      sendDataI2S();
      vscanEnd();
    }
    esp_rom_delay_us(230);
  }

  clean(3, 1);
  // vscanStart();

  if (!leaveOn)
    einkOff();

  return ESP_OK;
}

esp_err_t Inkplate5::display1b(bool leaveOn) {
  esp_err_t ret = einkOn();
  if (ret != ESP_OK) {
    ESP_LOGI(TAG, "Display is not on!");
    return ret;
  }

  clean(0, 1);
  clean(1, 11);
  clean(2, 1);
  clean(0, 11);
  clean(2, 1);
  clean(1, 11);
  clean(2, 1);
  clean(0, 11);

  memcpy(m_framebuffer, m_newFramebuffer, E_INK_WIDTH * E_INK_HEIGHT / 8);

  for (int k = 0; k < 3; k++) {
    uint8_t *memoryPtr = m_newFramebuffer;
    vscanStart();

    for (int i = 0; i < E_INK_HEIGHT; i++) {
      uint8_t *framebufferFlipped = (memoryPtr + E_INK_WIDTH / 8) - 1;
      for (int j = 0; j < (E_INK_WIDTH / 4); j += 4) {
        uint8_t dram1 = *(framebufferFlipped--);
        uint8_t dram2 = *(framebufferFlipped--);
        m_dmaLineBuffer[j] = LUTB[(dram2 >> 4) & 0x0F];
        m_dmaLineBuffer[j + 1] = LUTB[dram2 & 0x0F];
        m_dmaLineBuffer[j + 2] = LUTB[(dram1 >> 4) & 0x0F];
        m_dmaLineBuffer[j + 3] = LUTB[dram1 & 0x0F];
        memoryPtr += 2;
      }
      sendDataI2S();
      vscanEnd();
    }
    esp_rom_delay_us(230);
  }

  {
    uint8_t *memoryPtr = m_newFramebuffer;
    vscanStart();

    for (int i = 0; i < E_INK_HEIGHT; i++) {
      uint8_t *framebufferFlipped = (memoryPtr + E_INK_WIDTH / 8) - 1;

      for (int j = 0; j < (E_INK_WIDTH / 4); j += 4) {
        uint8_t dram1 = *(framebufferFlipped--);
        uint8_t dram2 = *(framebufferFlipped--);
        m_dmaLineBuffer[j] = LUT2[(dram2 >> 4) & 0x0F];
        m_dmaLineBuffer[j + 1] = LUT2[dram2 & 0x0F];
        m_dmaLineBuffer[j + 2] = LUT2[(dram1 >> 4) & 0x0F];
        m_dmaLineBuffer[j + 3] = LUT2[dram1 & 0x0F];
        memoryPtr += 2;
      }
      sendDataI2S();
      vscanEnd();
    }
    esp_rom_delay_us(230);
  }

  {
    vscanStart();

    for (int i = 0; i < E_INK_HEIGHT; i++) {
      for (int j = 0; j < (E_INK_WIDTH / 4); j += 4) {
        m_dmaLineBuffer[j] = 0;
        m_dmaLineBuffer[j + 1] = 0;
        m_dmaLineBuffer[j + 2] = 0;
        m_dmaLineBuffer[j + 3] = 0;
      }
      sendDataI2S();
      vscanEnd();
    }
    esp_rom_delay_us(230);
  }

  vscanStart();
  if (!leaveOn)
    einkOff();

  m_blockPartial = false;
  return ESP_OK;
}

void Inkplate5::gpioInit() {
  for (uint32_t i = 0; i < 256; ++i)
    m_pinLUT[i] = ((i & 0x03) << 4) | (((i & 0x0C) >> 2) << 18) |
                  (((i & 0x10) >> 4) << 23) | (((i & 0xE0) >> 5) << 25);

  gpio_set_direction(GPIO_NUM_12, GPIO_MODE_INPUT);
  gpio_set_direction(GPIO_NUM_13, GPIO_MODE_INPUT);
  gpio_set_direction(GPIO_NUM_14, GPIO_MODE_INPUT);
  gpio_set_direction(GPIO_NUM_15, GPIO_MODE_INPUT);

  expander1.setDirection(OE, IO_MODE_OUTPUT, true);
  expander1.setDirection(GMOD, IO_MODE_OUTPUT, true);
  expander1.setDirection(SPV, IO_MODE_OUTPUT, true);
  expander1.setDirection(WAKEUP, IO_MODE_OUTPUT, true);
  expander1.setDirection(PWRUP, IO_MODE_OUTPUT, true);
  expander1.setDirection(VCOM, IO_MODE_OUTPUT, true);

  expander1.setDirection(GPIO0_ENABLE, IO_MODE_OUTPUT, true);
  expander1.setLevel(GPIO0_ENABLE, 1, true);

  expander1.setDirection(IO_NUM_B1, IO_MODE_OUTPUT, true);
  expander1.setLevel(IO_NUM_B1, 0, true);

  expander1.setDirection(IO_NUM_B6, IO_MODE_OUTPUT, true);
  expander1.setLevel(IO_NUM_B6, 0, true);
  expander1.setDirection(IO_NUM_B7, IO_MODE_OUTPUT, true);
  expander1.setLevel(IO_NUM_B7, 0, true);

  expander1.setDirection(SD_PMOS_PIN, IO_MODE_INPUT, true);

  pinsAsOutputs();
}

void Inkplate5::clean(uint8_t c, uint8_t rep) {
  einkOn();
  uint8_t data = 0;
  if (c == 0)
    data = 0b10101010;
  else if (c == 1)
    data = 0b01010101;
  else if (c == 2)
    data = 0b00000000;
  else if (c == 3)
    data = 0b11111111;

  for (int i = 0; i < (E_INK_WIDTH / 4); i++)
    m_dmaLineBuffer[i] = data;

  m_dmaI2SDesc->size = (E_INK_WIDTH / 4) + 16;
  m_dmaI2SDesc->length = (E_INK_WIDTH / 4) + 16;
  m_dmaI2SDesc->sosf = 1;
  m_dmaI2SDesc->owner = 1;
  m_dmaI2SDesc->qe.stqe_next = 0;
  m_dmaI2SDesc->eof = 1;
  m_dmaI2SDesc->buf = m_dmaLineBuffer;
  m_dmaI2SDesc->offset = 0;

  for (int k = 0; k < rep; ++k) {
    vscanStart();
    for (int i = 0; i < E_INK_HEIGHT; ++i) {
      sendDataI2S();
      vscanEnd();
    }
    esp_rom_delay_us(230);
  }
}

void Inkplate5::pinsAsOutputs() {
  gpio_set_direction(GPIO_NUM_0, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_32, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_33, GPIO_MODE_OUTPUT);

  expander1.setDirection(IO_NUM_A0, IO_MODE_OUTPUT, true);
  expander1.setDirection(IO_NUM_A1, IO_MODE_OUTPUT, true);
  expander1.setDirection(IO_NUM_A2, IO_MODE_OUTPUT, true);

  setI2S1pin(0, I2S1O_BCK_OUT_IDX, 0);
  setI2S1pin(4, I2S1O_DATA_OUT0_IDX, 0);
  setI2S1pin(5, I2S1O_DATA_OUT1_IDX, 0);
  setI2S1pin(18, I2S1O_DATA_OUT2_IDX, 0);
  setI2S1pin(19, I2S1O_DATA_OUT3_IDX, 0);
  setI2S1pin(23, I2S1O_DATA_OUT4_IDX, 0);
  setI2S1pin(25, I2S1O_DATA_OUT5_IDX, 0);
  setI2S1pin(26, I2S1O_DATA_OUT6_IDX, 0);
  setI2S1pin(27, I2S1O_DATA_OUT7_IDX, 0);

  m_i2s->conf1.tx_stop_en = 1;
}

void Inkplate5::pinsZstate() {
  m_i2s->conf1.tx_stop_en = 0;

  gpio_set_direction(GPIO_NUM_2, GPIO_MODE_INPUT);
  gpio_set_direction(GPIO_NUM_32, GPIO_MODE_INPUT);
  gpio_set_direction(GPIO_NUM_33, GPIO_MODE_INPUT);

  expander1.setDirection(IO_NUM_A0, IO_MODE_INPUT, true);
  expander1.setDirection(IO_NUM_A1, IO_MODE_INPUT, true);
  expander1.setDirection(IO_NUM_A2, IO_MODE_INPUT, true);

  gpio_set_direction(GPIO_NUM_0, GPIO_MODE_INPUT);
  gpio_set_direction(GPIO_NUM_4, GPIO_MODE_INPUT);
  gpio_set_direction(GPIO_NUM_5, GPIO_MODE_INPUT);
  gpio_set_direction(GPIO_NUM_18, GPIO_MODE_INPUT);
  gpio_set_direction(GPIO_NUM_19, GPIO_MODE_INPUT);
  gpio_set_direction(GPIO_NUM_23, GPIO_MODE_INPUT);
  gpio_set_direction(GPIO_NUM_25, GPIO_MODE_INPUT);
  gpio_set_direction(GPIO_NUM_26, GPIO_MODE_INPUT);
  gpio_set_direction(GPIO_NUM_27, GPIO_MODE_INPUT);
}
