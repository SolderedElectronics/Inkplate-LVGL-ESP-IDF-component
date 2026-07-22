/**
 * @file Inkplate7.cpp
 * @author Fran Fodor for Soldered
 * @brief Driver for Inkplate 7 (Spectra7 panel) board.
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
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "string.h"

#include "freertos/FreeRTOS.h"

#include "Inkplate7.h"

#include <cmath>

// Peripherals defined in BoardCommon.cpp
extern PCAL expander1;
extern I2C i2c;

static const char *TAG = "INKPLATE7";

/* -------------------------------------------------------------------------- */
/*                              Public functions                              */
/* -------------------------------------------------------------------------- */

Inkplate7::Inkplate7(lv_display_render_mode_t mode)
    : BoardCommon(E_INK_WIDTH, E_INK_HEIGHT, 0, 0),
      m_spi(SPECTRA73_SPI_MOSI, SPECTRA73_SPI_SCK) {
  ESP_ERROR_CHECK(initBuffers());

  clearDisplay();

  setPanelPinsToLow();

  setPanelPower(false);

  rtc.begin(i2c.getBusHandle());

  lv_init();
  // Panel is native landscape (E_INK_WIDTH x E_INK_HEIGHT) - no swap needed,
  // unlike Inkplate13's portrait panel. The panel is mounted rotated 180
  // degrees, compensated via the forced setRotation(2) in Inkplate.cpp.
  const size_t buf_size = E_INK_WIDTH * E_INK_HEIGHT * sizeof(lv_color16_t);
  m_lvglBuf = (uint8_t *)heap_caps_malloc(buf_size, MALLOC_CAP_SPIRAM);
  if (!m_lvglBuf)
    ESP_LOGE(TAG, "Failed to allocate LVGL buffer");
  m_disp = lv_display_create(E_INK_WIDTH, E_INK_HEIGHT);
  lv_display_set_color_format(m_disp, LV_COLOR_FORMAT_RGB565);
  lv_display_set_buffers(m_disp, m_lvglBuf, NULL, buf_size, mode);
  lv_display_set_flush_cb(m_disp, display_flush_callback_inkplate7);
  lv_display_set_user_data(m_disp, this);

  m_dither.begin(palette7, paletteIndices7, 6, this);

  ESP_LOGI(TAG, "Initialization finished!");
}

double Inkplate7::readBattery() {
  // Read the pin on the battery MOSFET. If it's high, that means it's the
  // older version of the board that uses PMOS only. If it's low, it's the
  // newer board with both PMOS and NMOS.
  expander1.setDirection(SPECTRA73_BATT_MOSFET_PIN, IO_MODE_INPUT);
  int state = expander1.getLevel(SPECTRA73_BATT_MOSFET_PIN);
  expander1.setDirection(SPECTRA73_BATT_MOSFET_PIN, IO_MODE_OUTPUT);

  // If the input is pulled high, it's PMOS only.
  // If it's pulled low, it's PMOS and NMOS.
  if (state)
    expander1.setLevel(SPECTRA73_BATT_MOSFET_PIN, 0);
  else
    expander1.setLevel(SPECTRA73_BATT_MOSFET_PIN, 1);

  // Wait a little bit after a MOSFET enable.
  esp_rom_delay_us(5000);

  adc_oneshot_unit_handle_t adcHandle;
  adc_oneshot_unit_init_cfg_t initCfg = {};
  initCfg.unit_id = ADC_UNIT_1;
  adc_oneshot_new_unit(&initCfg, &adcHandle);

  adc_oneshot_chan_cfg_t chanCfg = {};
  chanCfg.atten = ADC_ATTEN_DB_12;
  chanCfg.bitwidth = ADC_BITWIDTH_12;
  adc_oneshot_config_channel(adcHandle, ADC_CHANNEL_0, &chanCfg);

  adc_cali_handle_t caliHandle = NULL;
  adc_cali_curve_fitting_config_t caliCfg = {};
  caliCfg.unit_id = ADC_UNIT_1;
  caliCfg.atten = ADC_ATTEN_DB_12;
  caliCfg.bitwidth = ADC_BITWIDTH_12;
  bool calibrated =
      (adc_cali_create_scheme_curve_fitting(&caliCfg, &caliHandle) == ESP_OK);

  int raw = 0, mv = 0;
  adc_oneshot_read(adcHandle, ADC_CHANNEL_0, &raw);
  if (calibrated) {
    adc_cali_raw_to_voltage(caliHandle, raw, &mv);
    adc_cali_delete_scheme_curve_fitting(caliHandle);
  }
  adc_oneshot_del_unit(adcHandle);

  // Turn off the MOSFET (and voltage divider).
  if (state)
    expander1.setLevel(SPECTRA73_BATT_MOSFET_PIN, 1);
  else
    expander1.setLevel(SPECTRA73_BATT_MOSFET_PIN, 0);

  // Calculate the voltage at the battery terminal (voltage is divided in
  // half by the voltage divider).
  return (double(mv) * 2.0 / 1000.0);
}

void Inkplate7::clearDisplay() {
  memset(m_framebufferColor, (INKPLATE_WHITE << 4) | INKPLATE_WHITE,
         E_INK_WIDTH * E_INK_HEIGHT / 2);
  ESP_LOGI(TAG, "Display cleared.");
}

void Inkplate7::fillDisplay() {
  memset(m_framebufferColor, (INKPLATE_BLACK << 4) | INKPLATE_BLACK,
         E_INK_WIDTH * E_INK_HEIGHT / 2);
  ESP_LOGI(TAG, "Display filled.");
}

/* -------------------------------------------------------------------------- */
/*                              Private functions                             */
/* -------------------------------------------------------------------------- */

esp_err_t Inkplate7::initBuffers() {
  m_framebufferColor = (uint8_t *)heap_caps_malloc(
      E_INK_WIDTH * E_INK_HEIGHT / 2, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  if (!m_framebufferColor)
    return ESP_ERR_NO_MEM;

  return ESP_OK;
}

esp_err_t Inkplate7::display3b(bool leaveOn) {
  setPanelPower(true);

  sendCommand(SPECTRA73_REGISTER_DTM, nullptr, 0);
  gpio_set_level(SPECTRA73_CS_PIN, 0);
  m_spi.sendData(m_framebufferColor, E_INK_WIDTH * E_INK_HEIGHT / 2,
                 SPECTRA73_DC_PIN);
  gpio_set_level(SPECTRA73_CS_PIN, 1);

  waitForEpd(60000);

  sendCommand(SPECTRA73_REGISTER_DRF, SPECTRA73_REGISTER_DRF_V,
              sizeof(SPECTRA73_REGISTER_DRF_V));
  waitForEpd(60000);

  if (!leaveOn)
    setPanelPower(false);

  return ESP_OK;
}

bool Inkplate7::waitForEpd(uint32_t timeout) {
  uint32_t elapsed = 0;
  const uint32_t STEP = 10;

  while (gpio_get_level(SPECTRA73_BUSYN_PIN) == 0) {
    if (elapsed >= timeout) {
      ESP_LOGE(TAG, "EPD busy timeout");
      return false;
    }
    vTaskDelay(pdMS_TO_TICKS(STEP));
    elapsed += STEP;
  }

  return true;
}

void Inkplate7::resetPanel() {
  gpio_set_level(SPECTRA73_RST_PIN, 0);
  vTaskDelay(pdMS_TO_TICKS(10));
  gpio_set_level(SPECTRA73_RST_PIN, 1);
  vTaskDelay(pdMS_TO_TICKS(20));
}

void Inkplate7::setIO() {
  gpio_set_direction(SPECTRA73_DC_PIN, GPIO_MODE_OUTPUT);
  gpio_set_direction(SPECTRA73_CS_PIN, GPIO_MODE_OUTPUT);
  gpio_set_direction(SPECTRA73_RST_PIN, GPIO_MODE_OUTPUT);
  gpio_set_direction(SPECTRA73_BUSYN_PIN, GPIO_MODE_INPUT);
  gpio_pullup_en(SPECTRA73_BUSYN_PIN);
  gpio_set_direction(SPECTRA73_PWR_EN, GPIO_MODE_OUTPUT);
  gpio_set_direction(SPECTRA73_BS0, GPIO_MODE_OUTPUT);
  gpio_set_direction(SPECTRA73_BS1, GPIO_MODE_OUTPUT);

  // BS0 and BS1 low select the 4-wire SPI interface mode on the panel.
  gpio_set_level(SPECTRA73_DC_PIN, 1);
  gpio_set_level(SPECTRA73_CS_PIN, 1);
  gpio_set_level(SPECTRA73_RST_PIN, 0);
  gpio_set_level(SPECTRA73_PWR_EN, 0);
  gpio_set_level(SPECTRA73_BS0, 0);
  gpio_set_level(SPECTRA73_BS1, 0);

  if (!m_spi.isInitialized())
    m_spi.init();
}

void Inkplate7::setPanelPower(bool state) {
  if (state == getPanelState())
    return;

  if (state) {
    setPanelPinsToLow();
    vTaskDelay(pdMS_TO_TICKS(50));

    setIO();

    gpio_set_level(SPECTRA73_PWR_EN, 1);
    vTaskDelay(pdMS_TO_TICKS(100));

    resetPanel();
    waitForEpd(60000);

    screenInit();

    sendCommand(SPECTRA73_REGISTER_PON, nullptr, 0);
    waitForEpd(60000);
  } else {
    sendCommand(SPECTRA73_REGISTER_POF, SPECTRA73_REGISTER_POF_V,
                sizeof(SPECTRA73_REGISTER_POF_V));
    waitForEpd(60000);

    gpio_set_level(SPECTRA73_PWR_EN, 0);

    gpio_set_direction(SPECTRA73_DC_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(SPECTRA73_CS_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(SPECTRA73_RST_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(SPECTRA73_BUSYN_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(SPECTRA73_PWR_EN, GPIO_MODE_INPUT);

    m_spi.deinit();
  }

  setPanelState(state);
}

void Inkplate7::setPanelPinsToLow() {
  const gpio_num_t pins[] = {SPECTRA73_DC_PIN,   SPECTRA73_CS_PIN,
                             SPECTRA73_RST_PIN,  SPECTRA73_BUSYN_PIN,
                             SPECTRA73_PWR_EN,   SPECTRA73_BS0,
                             SPECTRA73_BS1};
  for (auto p : pins) {
    gpio_set_direction(p, GPIO_MODE_OUTPUT);
    gpio_set_level(p, 0);
  }
}

void Inkplate7::sendCommand(uint8_t cmd, const uint8_t *data, int n) {
  gpio_set_level(SPECTRA73_CS_PIN, 0);
  m_spi.sendCommand(cmd, SPECTRA73_DC_PIN);
  gpio_set_level(SPECTRA73_CS_PIN, 1);

  if (data && n > 0) {
    gpio_set_level(SPECTRA73_CS_PIN, 0);
    m_spi.sendData((uint8_t *)data, n, SPECTRA73_DC_PIN);
    gpio_set_level(SPECTRA73_CS_PIN, 1);
  }
}

void Inkplate7::screenInit() {
  sendCommand(SPECTRA73_REGISTER_CMDH, SPECTRA73_REGISTER_CMDH_V,
              sizeof(SPECTRA73_REGISTER_CMDH_V));
  sendCommand(SPECTRA73_REGISTER_PWR, SPECTRA73_REGISTER_PWR_V,
              sizeof(SPECTRA73_REGISTER_PWR_V));
  sendCommand(SPECTRA73_REGISTER_PSR, SPECTRA73_REGISTER_PSR_V,
              sizeof(SPECTRA73_REGISTER_PSR_V));
  sendCommand(SPECTRA73_REGISTER_PFS, SPECTRA73_REGISTER_PFS_V,
              sizeof(SPECTRA73_REGISTER_PFS_V));
  sendCommand(SPECTRA73_REGISTER_BTST1, SPECTRA73_REGISTER_BTST1_V,
              sizeof(SPECTRA73_REGISTER_BTST1_V));
  sendCommand(SPECTRA73_REGISTER_BTST2, SPECTRA73_REGISTER_BTST2_V,
              sizeof(SPECTRA73_REGISTER_BTST2_V));
  sendCommand(SPECTRA73_REGISTER_BTST3, SPECTRA73_REGISTER_BTST3_V,
              sizeof(SPECTRA73_REGISTER_BTST3_V));
  sendCommand(SPECTRA73_REGISTER_IPC, SPECTRA73_REGISTER_IPC_V,
              sizeof(SPECTRA73_REGISTER_IPC_V));
  sendCommand(SPECTRA73_REGISTER_PLL, SPECTRA73_REGISTER_PLL_V,
              sizeof(SPECTRA73_REGISTER_PLL_V));
  sendCommand(SPECTRA73_REGISTER_TSE, SPECTRA73_REGISTER_TSE_V,
              sizeof(SPECTRA73_REGISTER_TSE_V));
  sendCommand(SPECTRA73_REGISTER_CDI, SPECTRA73_REGISTER_CDI_V,
              sizeof(SPECTRA73_REGISTER_CDI_V));
  sendCommand(SPECTRA73_REGISTER_TCON, SPECTRA73_REGISTER_TCON_V,
              sizeof(SPECTRA73_REGISTER_TCON_V));
  sendCommand(SPECTRA73_REGISTER_TRES, SPECTRA73_REGISTER_TRES_V,
              sizeof(SPECTRA73_REGISTER_TRES_V));
  sendCommand(SPECTRA73_REGISTER_VDCS, SPECTRA73_REGISTER_VDCS_V,
              sizeof(SPECTRA73_REGISTER_VDCS_V));
  sendCommand(SPECTRA73_REGISTER_T_VDCS, SPECTRA73_REGISTER_T_VDCS_V,
              sizeof(SPECTRA73_REGISTER_T_VDCS_V));
  sendCommand(SPECTRA73_REGISTER_AGID, SPECTRA73_REGISTER_AGID_V,
              sizeof(SPECTRA73_REGISTER_AGID_V));
  sendCommand(SPECTRA73_REGISTER_PWS, SPECTRA73_REGISTER_PWS_V,
              sizeof(SPECTRA73_REGISTER_PWS_V));
  sendCommand(SPECTRA73_REGISTER_CCSET, SPECTRA73_REGISTER_CCSET_V,
              sizeof(SPECTRA73_REGISTER_CCSET_V));
  sendCommand(SPECTRA73_REGISTER_TSSET, SPECTRA73_REGISTER_TSSET_V,
              sizeof(SPECTRA73_REGISTER_TSSET_V));
}

/* -------------------------------------------------------------------------- */
/*                             LVGL flush callback                            */
/* -------------------------------------------------------------------------- */

void display_flush_callback_inkplate7(lv_display_t *disp,
                                      const lv_area_t *area,
                                      uint8_t *px_map) {
  Inkplate7 *self = static_cast<Inkplate7 *>(lv_display_get_user_data(disp));

  int32_t w = lv_area_get_width(area);
  int32_t h = lv_area_get_height(area);

  if (self->m_ditherEnabled &&
      lv_display_get_render_mode(disp) == LV_DISPLAY_RENDER_MODE_FULL) {
    // Dither call uses logical LVGL dims (E_INK_WIDTH x E_INK_HEIGHT) -
    // writePixelInternal (via BoardCommon, rotation forced to 2 in
    // Inkplate.cpp) maps each logical pixel to the physical framebuffer.
    self->m_dither.ditherFramebuffer(px_map, E_INK_WIDTH, E_INK_HEIGHT);
  } else {
    for (int32_t y = 0; y < h; y++) {
      if ((y & 7) == 0)
        vTaskDelay(1);

      const uint8_t *src_row = px_map + (size_t)y * w * 2;

      for (int32_t x = 0; x < w; x++) {
        uint8_t lo = src_row[2 * x + 0];
        uint8_t hi = src_row[2 * x + 1];
        uint16_t pixel = (uint16_t)hi << 8 | lo;

        // Extract RGB565 components and scale to 0-255
        uint8_t r5 = (pixel >> 11) & 0x1F;
        uint8_t g6 = (pixel >> 5) & 0x3F;
        uint8_t b5 = pixel & 0x1F;

        int R = (r5 * 527 + 23) >> 6;
        int G = (g6 * 259 + 33) >> 6;
        int B = (b5 * 527 + 23) >> 6;

        // Convert to HSV
        float rf = R / 255.0f;
        float gf = G / 255.0f;
        float bf = B / 255.0f;

        float maxc = std::max(rf, std::max(gf, bf));
        float minc = std::min(rf, std::min(gf, bf));
        float delta = maxc - minc;

        float H = 0.0f;
        float S = (maxc == 0.0f) ? 0.0f : (delta / maxc);
        float V = maxc;

        if (delta > 0.0001f) {
          if (maxc == rf)
            H = 60.0f * std::fmod(((gf - bf) / delta), 6.0f);
          else if (maxc == gf)
            H = 60.0f * (((bf - rf) / delta) + 2.0f);
          else
            H = 60.0f * (((rf - gf) / delta) + 4.0f);
        }
        if (H < 0.0f)
          H += 360.0f;

        // Map to Inkplate 7 6-color palette via HSV classification.
        // Hardware color codes skip index 4 (see paletteIndices7 in
        // Inkplate7.h) - blue/green use codes 5/6, not 4/5.
        uint8_t color;
        if (S < 0.18f) {
          color = (V < 0.4f) ? INKPLATE_BLACK : INKPLATE_WHITE;
        } else {
          if (H >= 200.0f && H < 245.0f)
            color = 5; // blue
          else if (H >= 90.0f && H < 150.0f)
            color = 6; // green
          else if (H >= 45.0f && H < 90.0f)
            color = INKPLATE_YELLOW;
          else
            color = INKPLATE_RED;
        }

        int32_t sx = area->x1 + x;
        int32_t sy = area->y1 + y;
        self->writePixelInternal(sx, sy, color);
      }
    }
  }

  lv_display_flush_ready(disp);
}
