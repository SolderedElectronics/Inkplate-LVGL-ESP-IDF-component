/**
 * @file Inkplate10.cpp
 * @author Fran Fodor for Soldered
 * @brief Driver for Inkplate 10 board.
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
#include "nvs.h"
#include "nvs_flash.h"
#include "string.h"

#include "I2C.h"
#include "Inkplate10.h"
#include "TPS.h"
#include "lvgl.h"

// Peripherals defined in BoardCommon.cpp
extern PCAL expander2;
extern TPS tps;
extern I2C i2c;

static const char *TAG = "INKPLATE10";

/* -------------------------------------------------------------------------- */
/*                              Public functions                              */
/* -------------------------------------------------------------------------- */

Inkplate10::Inkplate10(lv_display_render_mode_t mode) : BoardCommon(E_INK_WIDTH, E_INK_HEIGHT, 12, 9) {
  ESP_ERROR_CHECK(initBuffers());
  calculateLUTs();
  gpioInit();
  blockGpioPins();
  ESP_ERROR_CHECK(pmicBegin());
  rtc.begin(i2c.getBusHandle());

  lv_init();
  const size_t buf_size = E_INK_WIDTH * E_INK_HEIGHT;  // L8 = 1 byte/pixel
  m_lvglBuf = (uint8_t *)heap_caps_malloc(buf_size, MALLOC_CAP_SPIRAM);
  if (!m_lvglBuf)
    ESP_LOGE(TAG, "Failed to allocate LVGL buffer");
  m_disp = lv_display_create(E_INK_WIDTH, E_INK_HEIGHT);
  lv_display_set_color_format(m_disp, LV_COLOR_FORMAT_L8);
  lv_display_set_buffers(m_disp, m_lvglBuf, NULL, buf_size, mode);
  lv_display_set_flush_cb(m_disp, display_flush_callback);
  lv_display_set_user_data(m_disp, this);
  m_dither.begin(this);

  ESP_LOGI(TAG, "Initialization finished!");
}

uint32_t Inkplate10::partialUpdate(bool forced, bool leaveOn) {
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
  uint8_t data = 0;
  uint32_t send;
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

  for (int k = 0; k < 5; k++) {
    vscanStart();
    n = (m_einkWidth * m_einkHeight / 4) - 1;

    for (int i = 0; i < m_einkHeight; i++) {
      data = *(m_waveformBuffer + n);
      send = m_pinLUT[data];
      hscanStart(send);
      n--;

      for (int j = 0; j < (m_einkWidth / 4) - 1; j++) {
        data = *(m_waveformBuffer + n);
        send = m_pinLUT[data];
        GPIO.out_w1ts = send | CL;
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

esp_err_t Inkplate10::einkOn() {
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

esp_err_t Inkplate10::einkOff() {
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

esp_err_t Inkplate10::setWaveform(uint8_t waveformNumber, bool burnToEEPROM) {
  if (waveformNumber < 1 || waveformNumber > 5)
    return ESP_ERR_INVALID_ARG;

  uint8_t index = waveformNumber - 1;
  esp_err_t ret = changeWaveform((uint8_t *)waveformList[index]);
  if (ret != ESP_OK)
    return ret;

  if (!burnToEEPROM)
    return ESP_OK;

  waveformData waveformEEPROM;
  waveformEEPROM.waveformId = INKPLATE10_WAVEFORM1 + index;
  memcpy(&waveformEEPROM.waveform, waveformList[index],
         sizeof(waveformEEPROM.waveform));
  waveformEEPROM.checksum = calculateChecksum(waveformEEPROM);

  return burnWaveformToEEPROM(waveformEEPROM);
}

esp_err_t Inkplate10::getWaveformFromEEPROM(struct waveformData *waveformData) {
  nvs_handle_t handle;
  esp_err_t ret;

  ret = nvs_open("inkplate", NVS_READONLY, &handle);
  if (ret != ESP_OK)
    return ret;

  size_t size = sizeof(struct waveformData);
  ret = nvs_get_blob(handle, "waveform", waveformData, &size);
  nvs_close(handle);
  if (ret != ESP_OK)
    return ret;

  return (calculateChecksum(*waveformData) == waveformData->checksum)
             ? ESP_OK
             : ESP_ERR_INVALID_CRC;
}

esp_err_t Inkplate10::changeWaveform(uint8_t *waveform) {
  if (!waveform)
    return ESP_ERR_INVALID_ARG;
  memcpy(waveform3Bit, waveform, sizeof(waveform3Bit));
  calculateLUTs();
  return ESP_OK;
}

/* -------------------------------------------------------------------------- */
/*                              Private functions                             */
/* -------------------------------------------------------------------------- */

esp_err_t Inkplate10::initBuffers() {
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

void Inkplate10::calculateLUTs() {
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

esp_err_t Inkplate10::display3b(bool leaveOn) {
  esp_err_t ret = einkOn();
  if (ret != ESP_OK) {
    ESP_LOGI(TAG, "Display is not on!");
    return ret;
  }

  clean(1, 1);
  clean(0, 10);
  clean(2, 1);
  clean(1, 10);
  clean(2, 1);
  clean(0, 10);
  clean(2, 1);
  clean(1, 10);

  for (int k = 0; k < 9; k++) {
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

esp_err_t Inkplate10::display1b(bool leaveOn) {
  esp_err_t ret = einkOn();
  if (ret != ESP_OK) {
    ESP_LOGI(TAG, "Display is not on!");
    return ret;
  }

  clean(0, 1);
  clean(1, 18);
  clean(2, 1);
  clean(0, 18);
  clean(2, 1);
  clean(1, 18);
  clean(2, 1);
  clean(0, 18);
  clean(2, 1);

  memcpy(m_framebuffer, m_newFramebuffer, E_INK_WIDTH * E_INK_HEIGHT / 8);

  for (int k = 0; k < 5; k++) {
    uint32_t pos = (E_INK_HEIGHT * E_INK_WIDTH / 8) - 1;
    vscanStart();

    for (int i = 0; i < E_INK_HEIGHT; i++) {
      uint8_t dram = *(m_framebuffer + pos);
      uint8_t data = LUTB[(dram >> 4) & 0x0F];
      hscanStart(m_pinLUT[data]);
      data = LUTB[dram & 0x0F];
      GPIO.out_w1ts = m_pinLUT[data] | CL;
      GPIO.out_w1tc = DATA | CL;
      pos--;

      for (int j = 0; j < ((E_INK_WIDTH / 8) - 1); j++) {
        dram = *(m_framebuffer + pos);
        data = LUTB[(dram >> 4) & 0x0F];
        GPIO.out_w1ts = m_pinLUT[data] | CL;
        GPIO.out_w1tc = DATA | CL;
        data = LUTB[dram & 0x0F];
        GPIO.out_w1ts = m_pinLUT[data] | CL;
        GPIO.out_w1tc = DATA | CL;
        pos--;
      }
      GPIO.out_w1ts = CL;
      GPIO.out_w1tc = DATA | CL;
      vscanEnd();
    }
    esp_rom_delay_us(230);
  }

  clean(2, 2);
  clean(3, 1);
  vscanStart();

  if (!leaveOn)
    einkOff();

  m_blockPartial = false;
  return ESP_OK;
}

void Inkplate10::hscanStart(uint32_t data) {
  SPH_CLEAR;
  GPIO.out_w1ts = data | CL;
  GPIO.out_w1tc = DATA | CL;
  SPH_SET;
  CKV_SET;
}

void Inkplate10::gpioInit() {
  expander1.setLevel(IO_NUM_B1, 0, true);

  expander1.setDirection(VCOM, IO_MODE_OUTPUT, true);
  expander1.setDirection(PWRUP, IO_MODE_OUTPUT, true);
  expander1.setDirection(WAKEUP, IO_MODE_OUTPUT, true);
  expander1.setDirection(GPIO0_ENABLE, IO_MODE_OUTPUT, true);
  expander1.setLevel(GPIO0_ENABLE, 1, true);

  expander1.setDirection(IO_NUM_B6, IO_MODE_OUTPUT, true);
  expander1.setDirection(IO_NUM_B7, IO_MODE_OUTPUT, true);
  expander1.setLevel(IO_NUM_B6, 0, true);
  expander1.setLevel(IO_NUM_B7, 0, true);

  gpio_set_direction(GPIO_NUM_12, GPIO_MODE_INPUT);
  gpio_set_direction(GPIO_NUM_13, GPIO_MODE_INPUT);
  gpio_set_direction(GPIO_NUM_14, GPIO_MODE_INPUT);
  gpio_set_direction(GPIO_NUM_15, GPIO_MODE_INPUT);

  expander1.setDirection(SD_PMOS_PIN, IO_MODE_INPUT, true);

  gpio_set_direction(GPIO_NUM_0, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_32, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_33, GPIO_MODE_OUTPUT);
  expander1.setDirection(OE, IO_MODE_OUTPUT, true);
  expander1.setDirection(GMOD, IO_MODE_OUTPUT, true);
  expander1.setDirection(SPV, IO_MODE_OUTPUT, true);

  gpio_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_5, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_18, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_19, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_23, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_25, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_26, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_27, GPIO_MODE_OUTPUT);

  expander1.setDirection(IO_NUM_B2, IO_MODE_OUTPUT, true);
  expander1.setDirection(IO_NUM_B3, IO_MODE_OUTPUT, true);
  expander1.setDirection(IO_NUM_B4, IO_MODE_OUTPUT, true);
  expander1.setLevel(IO_NUM_B2, 0, true);
  expander1.setLevel(IO_NUM_B3, 0, true);
  expander1.setLevel(IO_NUM_B4, 0, true);

  expander1.setDirection(IO_NUM_B1, IO_MODE_OUTPUT, true);
  expander1.setLevel(IO_NUM_B1, 0, true);

  for (uint32_t i = 0; i < 256; ++i)
    m_pinLUT[i] = ((i & 0x03) << 4) | (((i & 0x0C) >> 2) << 18) |
                  (((i & 0x10) >> 4) << 23) | (((i & 0xE0) >> 5) << 25);

  expander2.setPort(IO_PORT_0, 0x00);
  expander2.setPort(IO_PORT_1, 0x00);
  expander2.setPortDirection(IO_PORT_0, 0x00);
  expander2.setPortDirection(IO_PORT_1, 0x00);
}

void Inkplate10::clean(uint8_t c, uint8_t rep) {
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

  uint32_t send = m_pinLUT[data];
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
      GPIO.out_w1ts = CL;
      GPIO.out_w1tc = CL;
      vscanEnd();
    }
    esp_rom_delay_us(230);
  }
}

void Inkplate10::pinsAsOutputs() {
  gpio_set_direction(GPIO_NUM_0, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_32, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_33, GPIO_MODE_OUTPUT);

  expander1.setDirection(OE, IO_MODE_OUTPUT, true);
  expander1.setDirection(GMOD, IO_MODE_OUTPUT, true);
  expander1.setDirection(SPV, IO_MODE_OUTPUT, true);

  gpio_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_5, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_18, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_19, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_23, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_25, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_26, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_27, GPIO_MODE_OUTPUT);
}

void Inkplate10::pinsZstate() {
  gpio_set_direction(GPIO_NUM_2, GPIO_MODE_INPUT);
  gpio_set_direction(GPIO_NUM_32, GPIO_MODE_INPUT);
  gpio_set_direction(GPIO_NUM_33, GPIO_MODE_INPUT);

  expander1.setDirection(OE, IO_MODE_INPUT, true);
  expander1.setDirection(GMOD, IO_MODE_INPUT, true);
  expander1.setDirection(SPV, IO_MODE_INPUT, true);

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

uint8_t Inkplate10::calculateChecksum(struct waveformData waveformData) {
  uint8_t *d = (uint8_t *)&waveformData;
  uint16_t sum = 0;
  int n = sizeof(struct waveformData) - 1;
  for (int i = 0; i < n; i++)
    sum += d[i];
  return sum % 256;
}

esp_err_t Inkplate10::burnWaveformToEEPROM(struct waveformData waveformData) {
  nvs_handle_t handle;
  esp_err_t ret;

  ret = nvs_open("inkplate", NVS_READWRITE, &handle);
  if (ret != ESP_OK)
    return ret;

  ret = nvs_set_blob(handle, "waveform", &waveformData,
                     sizeof(struct waveformData));
  if (ret == ESP_OK)
    ret = nvs_commit(handle);

  nvs_close(handle);
  return ret;
}

void IRAM_ATTR display_flush_callback(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    Inkplate10 *self = static_cast<Inkplate10 *>(lv_display_get_user_data(disp));

    int32_t w = lv_area_get_width(area);
    int32_t h = lv_area_get_height(area);

    if (w <= 0 || h <= 0 || px_map == nullptr || area->x1 < 0 || area->y1 < 0 ||
        area->x2 >= E_INK_WIDTH || area->y2 >= E_INK_HEIGHT)
    {
        lv_display_flush_ready(disp);
        return;
    }

    bool is3bit = (self->getDisplayMode() == GRAYSCALE);

    if (self->m_ditherEnabled && lv_display_get_render_mode(disp) == LV_DISPLAY_RENDER_MODE_FULL)
    {
        self->m_dither.ditherFramebuffer(px_map, E_INK_WIDTH, E_INK_HEIGHT, is3bit ? 1 : 0);
    }
    else
    {
        uint8_t *buffer1b = self->m_newFramebuffer;
        uint8_t *buffer3b = self->m_framebufferColor;

        const int width_bytes_1b = E_INK_WIDTH / 8;
        const int width_bytes_3b = E_INK_WIDTH / 2;

        const uint8_t *maskLUT = pixelMaskLUT;
        const uint8_t *maskGLUT = pixelMaskGLUT;

        for (int32_t y = 0; y < h; y++)
        {
            int32_t screen_y = area->y1 + y;
            const uint8_t *src_row = px_map + (y * w);

            if (is3bit)
            {
                uint8_t *dst_row = buffer3b + (width_bytes_3b * screen_y);

                for (int32_t x = 0; x < w; x++)
                {
                    int32_t screen_x = area->x1 + x;
                    if (screen_x >= E_INK_WIDTH)
                        break;

                    uint8_t gray3 = src_row[x] >> 5;
                    int x_byte = screen_x / 2;
                    int x_sub = screen_x % 2;

                    uint8_t temp = dst_row[x_byte];
                    dst_row[x_byte] = (maskGLUT[x_sub] & temp) | (x_sub ? gray3 : (gray3 << 4));
                }
            }
            else
            {
                uint8_t *dst_row = buffer1b + (width_bytes_1b * screen_y);

                for (int32_t x = 0; x < w; x++)
                {
                    int32_t screen_x = area->x1 + x;
                    if (screen_x >= E_INK_WIDTH)
                        break;

                    uint8_t gray = src_row[x];
                    uint8_t bit = (gray < 128) ? 1 : 0;

                    int x_byte = screen_x / 8;
                    int x_sub = screen_x % 8;

                    uint8_t temp = dst_row[x_byte];
                    dst_row[x_byte] = (~maskLUT[x_sub] & temp) | (bit ? maskLUT[x_sub] : 0);
                }
            }
        }
    }

    lv_display_flush_ready(disp);
}