/**
 * @file Inkplate4.cpp
 * @author Fran Fodor for Soldered
 * @brief Driver for Inkplate 4 board.
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
#include "driver/i2c_master.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "string.h"

#include "I2C.h"
#include "Inkplate4.h"
#include "TPS.h"

// Peripherals defined in BoardCommon.cpp
extern PCAL expander2;
extern TPS tps;
extern I2C i2c;

static const char *TAG = "INKPLATE4";

/* -------------------------------------------------------------------------- */
/*                              Public functions                              */
/* -------------------------------------------------------------------------- */

Inkplate4::Inkplate4()
    : BoardCommon(E_INK_WIDTH, E_INK_HEIGHT, 0,
                  0) //, frontlight(i2c, expander1)
{
  ESP_ERROR_CHECK(initBuffers());
  calculateLUTs();
  gpioInit();
  blockGpioPins();
  ESP_ERROR_CHECK(pmicBegin());

  i2c_master_bus_handle_t bus = i2c.getBusHandle();
  rtc.begin(bus);
  apds.begin(bus);
  bq.begin(bus);
  lsm.begin(bus);
  bme.begin(bus);
  touchscreen.begin(i2c, expander2, true);
  frontlight.begin(i2c, expander1, FRONTLIGHT_EN);
  frontlight.setState(false);

  ESP_LOGI(TAG, "Initialization finished!");
}

uint32_t Inkplate4::partialUpdate(bool forced, bool leaveOn) {
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

  uint32_t position = (m_einkWidth * m_einkHeight / 8) - 1;
  uint32_t n = (m_einkWidth * m_einkHeight / 4) - 1;
  uint8_t diffWhite, diffBlack;
  uint32_t send;
  uint8_t data = 0;
  uint32_t changeCount = 0;

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

  for (int k = 0; k < 9; k++) {
    vscanStart();
    n = (m_einkWidth * m_einkHeight / 4) - 1;

    for (int i = 0; i < m_einkHeight; i++) {
      data = *(m_waveformBuffer + n);
      send = m_pinLUT[data];
      hscanStart(send);
      n--;

      for (int j = 0; j < (m_einkWidth / 4) - 1; j++) {
        data = *(m_waveformBuffer + n);
        GPIO.out_w1ts = (m_pinLUT[data]) | CL;
        GPIO.out_w1tc = DATA | CL;
        n--;
      }
      GPIO.out_w1ts = send | CL;
      GPIO.out_w1tc = DATA | CL;
      vscanEnd();
    }
    esp_rom_delay_us(230);
  }

  clean(2, 2);
  clean(3, 1);
  vscanStart();

  if (einkOn() != ESP_OK)
    einkOff();

  memcpy(m_framebuffer, m_newFramebuffer, m_einkWidth * m_einkHeight / 8);

  if (m_partialUpdateLimiter != 0)
    m_partialUpdateCounter++;

  return changeCount;
}

esp_err_t Inkplate4::einkOn() {
  if (getPanelState())
    return ESP_OK;

  WAKEUP_SET;
  esp_rom_delay_us(5000);

  tps.enableRails();
  tps.setPowerUpSequence(TPS_PWRUP_SEQ);
  tps.setPowerDownSequence(TPS_PWRDN_SEQ);

  pinsAsOutputs();
  LE_CLEAR;
  CL_CLEAR;

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

esp_err_t Inkplate4::einkOff() {
  if (!getPanelState())
    return ESP_OK;

  VCOM_CLEAR;
  OE_CLEAR;
  GMOD_CLEAR;
  GPIO.out &= ~(DATA | LE | CL);

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

esp_err_t Inkplate4::initBuffers() {
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

void Inkplate4::calculateLUTs() {
  for (int j = 0; j < 9; ++j)
    for (int i = 0; i < 256; ++i) {
      uint8_t z =
          (waveform3Bit[i & 0x07][j] << 2) | (waveform3Bit[(i >> 4) & 0x07][j]);
      m_glut[j * 256 + i] =
          ((z & 0B00000011) << 4) | (((z & 0B00001100) >> 2) << 18) |
          (((z & 0B00010000) >> 4) << 23) | (((z & 0B11100000) >> 5) << 25);
      z = ((waveform3Bit[i & 0x07][j] << 2) |
           (waveform3Bit[(i >> 4) & 0x07][j]))
          << 4;
      m_glut2[j * 256 + i] =
          ((z & 0B00000011) << 4) | (((z & 0B00001100) >> 2) << 18) |
          (((z & 0B00010000) >> 4) << 23) | (((z & 0B11100000) >> 5) << 25);
    }
}

esp_err_t Inkplate4::display3b(bool leaveOn) {
  esp_err_t ret = einkOn();
  if (ret != ESP_OK) {
    ESP_LOGI(TAG, "Display is not on!");
    return ret;
  }

  clean(0, 5);
  clean(1, 15);
  clean(0, 15);
  clean(1, 15);
  clean(0, 15);

  for (int k = 0; k < 8; k++) {
    uint8_t *dp = m_framebufferColor + E_INK_WIDTH * E_INK_HEIGHT / 2;
    vscanStart();

    for (int i = 0; i < E_INK_HEIGHT; i++) {
      uint32_t t = m_glut2[k * 256 + (*(--dp))];
      t |= m_glut[k * 256 + (*(--dp))];
      hscanStart(t);
      t = m_glut2[k * 256 + (*(--dp))];
      t |= m_glut[k * 256 + (*(--dp))];
      GPIO.out_w1ts = t | CL;
      GPIO.out_w1tc = DATA | CL;

      for (int j = 0; j < ((E_INK_WIDTH / 8) - 1); j++) {
        t = m_glut2[k * 256 + (*(--dp))];
        t |= m_glut[k * 256 + (*(--dp))];
        GPIO.out_w1ts = t | CL;
        GPIO.out_w1tc = DATA | CL;
        t = m_glut2[k * 256 + (*(--dp))];
        t |= m_glut[k * 256 + (*(--dp))];
        GPIO.out_w1ts = t | CL;
        GPIO.out_w1tc = DATA | CL;
      }

      GPIO.out_w1ts = CL;
      GPIO.out_w1tc = DATA | CL;
      vscanEnd();
    }
    esp_rom_delay_us(230);
  }

  clean(3, 1);
  vscanStart();

  if (!leaveOn)
    einkOff();

  return ESP_OK;
}

esp_err_t Inkplate4::display1b(bool leaveOn) {
  esp_err_t ret = einkOn();
  if (ret != ESP_OK) {
    ESP_LOGI(TAG, "Display is not on!");
    return ret;
  }

  clean(0, 5);
  clean(1, 15);
  clean(0, 15);
  clean(1, 15);
  clean(0, 15);

  memcpy(m_framebuffer, m_newFramebuffer, E_INK_WIDTH * E_INK_HEIGHT / 8);

  for (int k = 0; k < 10; ++k) {
    uint8_t *ptr = m_framebuffer + (E_INK_WIDTH * E_INK_HEIGHT / 8) - 1;
    vscanStart();
    for (int i = 0; i < E_INK_HEIGHT; ++i) {
      uint8_t dram = *(ptr--);
      uint32_t send = m_pinLUT[LUTB[dram >> 4]];
      hscanStart(send);
      send = m_pinLUT[LUTB[dram & 0x0F]];
      GPIO.out_w1ts = send | CL;
      GPIO.out_w1tc = DATA | CL;

      for (int j = 0; j < (E_INK_WIDTH / 8) - 1; ++j) {
        dram = *(ptr--);
        send = m_pinLUT[LUTB[dram >> 4]];
        GPIO.out_w1ts = send | CL;
        GPIO.out_w1tc = DATA | CL;
        send = m_pinLUT[LUTB[dram & 0x0F]];
        GPIO.out_w1ts = send | CL;
        GPIO.out_w1tc = DATA | CL;
      }
      GPIO.out_w1ts = send | CL;
      GPIO.out_w1tc = DATA | CL;
      vscanEnd();
    }
    esp_rom_delay_us(230);
  }

  // Final pass — drive pixels to their settled state
  {
    uint8_t *ptr = m_framebuffer + (E_INK_WIDTH * E_INK_HEIGHT / 8) - 1;
    vscanStart();
    for (int i = 0; i < E_INK_HEIGHT; ++i) {
      uint8_t dram = *(ptr);
      uint32_t send = m_pinLUT[LUT2[dram >> 4]];
      hscanStart(send);
      send = m_pinLUT[LUT2[dram & 0x0F]];
      GPIO.out_w1ts = send | CL;
      GPIO.out_w1tc = DATA | CL;
      ptr--;

      for (int j = 0; j < (E_INK_WIDTH / 8) - 1; ++j) {
        dram = *(ptr);
        send = m_pinLUT[LUT2[dram >> 4]];
        GPIO.out_w1ts = send | CL;
        GPIO.out_w1tc = DATA | CL;
        send = m_pinLUT[LUT2[dram & 0x0F]];
        GPIO.out_w1ts = send | CL;
        GPIO.out_w1tc = DATA | CL;
        ptr--;
      }
      GPIO.out_w1ts = send | CL;
      GPIO.out_w1tc = DATA | CL;
      vscanEnd();
    }
    esp_rom_delay_us(230);
  }

  clean(2, 1);
  clean(3, 1);
  vscanStart();

  if (!leaveOn)
    einkOff();

  m_blockPartial = false;
  return ESP_OK;
}

void Inkplate4::hscanStart(uint32_t data) {
  SPH_CLEAR;
  GPIO.out_w1ts = data | CL;
  GPIO.out_w1tc = DATA | CL;
  SPH_SET;
  CKV_SET;
}

void Inkplate4::gpioInit() {
  for (uint32_t i = 0; i < 256; ++i)
    m_pinLUT[i] = ((i & 0x03) << 4) | (((i & 0x0C) >> 2) << 18) |
                  (((i & 0x10) >> 4) << 23) | (((i & 0xE0) >> 5) << 25);

  gpio_set_direction(GPIO_NUM_12, GPIO_MODE_INPUT);
  gpio_set_direction(GPIO_NUM_13, GPIO_MODE_INPUT);
  gpio_set_direction(GPIO_NUM_14, GPIO_MODE_INPUT);
  gpio_set_direction(GPIO_NUM_15, GPIO_MODE_INPUT);

  gpio_set_direction(GPIO_NUM_0, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_32, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_33, GPIO_MODE_OUTPUT);
  expander1.setDirection(OE, IO_MODE_OUTPUT, true);
  expander1.setDirection(GMOD, IO_MODE_OUTPUT, true);
  expander1.setDirection(SPV, IO_MODE_OUTPUT, true);
  expander1.setDirection(WAKEUP, IO_MODE_OUTPUT, true);
  expander1.setDirection(PWRUP, IO_MODE_OUTPUT, true);
  expander1.setDirection(VCOM, IO_MODE_OUTPUT, true);

  gpio_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_5, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_18, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_19, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_23, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_25, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_26, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_27, GPIO_MODE_OUTPUT);

  expander1.setDirection(GPIO0_ENABLE, IO_MODE_OUTPUT, true);
  expander1.setLevel(GPIO0_ENABLE, 1, true);

  expander1.setDirection(SD_PMOS_PIN, IO_MODE_INPUT, true);

  expander2.setPort(IO_PORT_0, 0x00);
  expander2.setPort(IO_PORT_1, 0x00);
  expander2.setPortDirection(IO_PORT_0, 0x00);
  expander2.setPortDirection(IO_PORT_1, 0x00);

  expander2.setDirection(TOUCHSCREEN_EN, IO_MODE_OUTPUT, true);
  expander2.setLevel(TOUCHSCREEN_EN, 1, true);

  pinsAsOutputs();
}

void Inkplate4::clean(uint8_t c, uint8_t rep) {
  einkOn();
  uint8_t data = 0;
  if (c == 0)
    data = 0B10101010;
  else if (c == 1)
    data = 0B01010101;
  else if (c == 2)
    data = 0B00000000;
  else if (c == 3)
    data = 0B11111111;

  uint32_t send =
      ((data & 0B00000011) << 4) | (((data & 0B00001100) >> 2) << 18) |
      (((data & 0B00010000) >> 4) << 23) | (((data & 0B11100000) >> 5) << 25);

  for (int k = 0; k < rep; ++k) {
    vscanStart();
    for (int i = 0; i < E_INK_HEIGHT; ++i) {
      hscanStart(send);
      GPIO.out_w1ts = send | CL;
      GPIO.out_w1tc = CL;
      for (int j = 0; j < ((E_INK_WIDTH / 8) - 1); ++j) {
        GPIO.out_w1ts = CL;
        GPIO.out_w1tc = CL;
        GPIO.out_w1ts = CL;
        GPIO.out_w1tc = CL;
      }
      GPIO.out_w1ts = send | CL;
      GPIO.out_w1tc = DATA | CL;
      vscanEnd();
    }
    esp_rom_delay_us(230);
  }
}

void Inkplate4::pinsAsOutputs() {
  gpio_set_direction(GPIO_NUM_0, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_32, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_33, GPIO_MODE_OUTPUT);

  expander1.setDirection(IO_NUM_A0, IO_MODE_OUTPUT, true);
  expander1.setDirection(IO_NUM_A1, IO_MODE_OUTPUT, true);
  expander1.setDirection(IO_NUM_A2, IO_MODE_OUTPUT, true);

  gpio_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_5, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_18, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_19, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_23, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_25, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_26, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_27, GPIO_MODE_OUTPUT);
}

void Inkplate4::pinsZstate() {
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
