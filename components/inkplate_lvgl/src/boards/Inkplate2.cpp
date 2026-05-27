/**
 * @file Inkplate2.cpp
 * @author Fran Fodor for Soldered
 * @brief Driver for Inkplate 2 board.
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

#include "Inkplate2.h"
#include "driver/gpio.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "lvgl.h"
#include "string.h"

static const char *TAG = "Inkplate2";

/* -------------------------------------------------------------------------- */
/*                              Public functions                              */
/* -------------------------------------------------------------------------- */

Inkplate2::Inkplate2(lv_display_render_mode_t mode)
    : m_spi(EPAPER_DIN, EPAPER_CLK) {
  setRotation(3);

  m_framebufferColor = (uint8_t *)heap_caps_malloc(
      E_INK_WIDTH * E_INK_HEIGHT / 4, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  if (!m_framebufferColor)
    ESP_LOGE(TAG, "Failed to allocate framebuffer");

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

  if (!setPanelDeepSleep(false))
    ESP_LOGE(TAG, "Panel init failed");

  setPanelDeepSleep(true);

  lv_init();
  // Always use RGB565 (2 bytes/pixel) regardless of LV_COLOR_DEPTH in sdkconfig
  const size_t buf_size = E_INK_HEIGHT * E_INK_WIDTH * 2;
  m_lvglBuf = (uint8_t *)heap_caps_malloc(buf_size, MALLOC_CAP_SPIRAM);
  m_disp = lv_display_create(E_INK_HEIGHT, E_INK_WIDTH);
  lv_display_set_color_format(m_disp, LV_COLOR_FORMAT_RGB565);
  lv_display_set_buffers(m_disp, m_lvglBuf, NULL, buf_size, mode);
  lv_display_set_flush_cb(m_disp, display_flush_callback);
  lv_display_set_user_data(m_disp, this);

  // Inkplate 2 palette: Black, White, Red in RGB565
  static uint16_t palette[3] = {0x0000, 0xFFFF, 0xF800};
  static uint8_t paletteIndices[3] = {INKPLATE2_BLACK, INKPLATE2_WHITE,
                                      INKPLATE2_RED};
  m_dither.begin(palette, paletteIndices, 3, this);
}

void Inkplate2::writePixelInternal(int16_t x, int16_t y, uint16_t color) {
  if (color > 2)
    return;

  uint8_t r = getRotation();

  switch (r) {
  case 1:
    _swap_int16_t(x, y);
    x = m_einkWidth - x - 1;
    break;
  case 2:
    x = m_einkWidth - x - 1;
    y = m_einkHeight - y - 1;
    break;
  case 3:
    _swap_int16_t(x, y);
    y = m_einkHeight - y - 1;
    break;
  default:
    break;
  }

  if (x < 0 || y < 0 || x >= m_einkWidth || y >= m_einkHeight)
    return;

  int x1 = x / 8;
  int xSub = x % 8;
  int position = (E_INK_WIDTH / 8) * y + x1;

  // Set both planes to 1 first (clear)
  *(m_framebufferColor + position) |= pixelMaskLUT[7 - xSub];
  *(m_framebufferColor + (E_INK_WIDTH * E_INK_HEIGHT / 8) + position) |=
      pixelMaskLUT[7 - xSub];

  if (color < 2) {
    *(m_framebufferColor + position) &= ~(color << (7 - xSub));
  } else {
    *(m_framebufferColor + (E_INK_WIDTH * E_INK_HEIGHT / 8) + position) &=
        ~pixelMaskLUT[7 - xSub];
  }
}

esp_err_t Inkplate2::display(bool leaveOn) {
  const size_t plane_bytes = E_INK_WIDTH * E_INK_HEIGHT / 8;

  setPanelDeepSleep(false);
  vTaskDelay(pdMS_TO_TICKS(20));

  sendCommand(0x10);
  sendData(m_framebufferColor, plane_bytes);

  sendCommand(0x13);
  sendData(m_framebufferColor + plane_bytes, plane_bytes);

  sendCommand(0x11);
  sendData((uint8_t)0x00);
  sendCommand(0x12);
  esp_rom_delay_us(500);
  waitForEpd(60000);

  if (!leaveOn)
    setPanelDeepSleep(true);

  return ESP_OK;
}

void Inkplate2::clearDisplay() {
  if (m_framebufferColor)
    memset(m_framebufferColor, 0xFF, E_INK_WIDTH * E_INK_HEIGHT / 4);
}

void Inkplate2::fillDisplay() {
  if (m_framebufferColor)
    memset(m_framebufferColor, 0x00, E_INK_WIDTH * E_INK_HEIGHT / 4);
}

/* -------------------------------------------------------------------------- */
/*                              Private functions                             */
/* -------------------------------------------------------------------------- */

bool Inkplate2::waitForEpd(uint32_t timeout) {
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

void Inkplate2::resetPanel() {
  gpio_set_level(EPAPER_RST_PIN, 0);
  vTaskDelay(pdMS_TO_TICKS(100));
  gpio_set_level(EPAPER_RST_PIN, 1);
  vTaskDelay(pdMS_TO_TICKS(100));
}

void Inkplate2::sendCommand(uint8_t command) {
  m_spi.sendCommand(command, EPAPER_DC_PIN);
}

void Inkplate2::sendData(uint8_t *data, int n) {
  m_spi.sendData(data, n, EPAPER_DC_PIN);
}

void Inkplate2::sendData(uint8_t data) { m_spi.sendData(data, EPAPER_DC_PIN); }

void display_flush_callback(lv_display_t *disp, const lv_area_t *area,
                            uint8_t *px_map) {
  Inkplate2 *self = (Inkplate2 *)lv_display_get_user_data(disp);

  int32_t w = lv_area_get_width(area);
  int32_t h = lv_area_get_height(area);

  if (self->m_ditherEnabled) {
    self->m_dither.ditherFramebuffer(px_map, w, h);
  } else {
    // Direct threshold conversion from RGB565 (always set via
    // lv_display_set_color_format)
    for (int32_t y = 0; y < h; y++) {
      if ((y & 7) == 0)
        vTaskDelay(1);
      const uint8_t *row = px_map + (size_t)y * w * 2;
      for (int32_t x = 0; x < w; x++) {
        const uint8_t *p = row + x * 2;
        uint16_t px = p[0] | ((uint16_t)p[1] << 8);
        uint8_t r5 = (px >> 11) & 0x1F;
        uint8_t g6 = (px >> 5) & 0x3F;
        uint8_t b5 = px & 0x1F;

        uint8_t color;
        if (r5 > 20 && g6 < 16 && b5 < 16) {
          color = INKPLATE2_RED;
        } else {
          color = ((r5 + g6 + b5) < 63) ? INKPLATE2_BLACK : INKPLATE2_WHITE;
        }
        self->writePixelInternal(area->x1 + x, area->y1 + y, color);
      }
    }
  }

  lv_display_flush_ready(disp);
}

bool Inkplate2::setPanelDeepSleep(bool sleep) {
  if (!sleep) {
    // Wake
    if (!m_spi.isInitialized())
      m_spi.init();

    gpio_set_direction(EPAPER_BUSY_PIN, GPIO_MODE_INPUT);
    gpio_pullup_en(EPAPER_BUSY_PIN);
    resetPanel();

    sendCommand(0x04); // Power on
    if (!waitForEpd(BUSY_TIMEOUT_MS))
      return false;

    sendCommand(0x00);       // Panel setting
    sendData((uint8_t)0x0f); // LUT from OTP
    sendData((uint8_t)0x89); // Temp sensor, boost, timing

    sendCommand(0x61); // Resolution setting
    sendData((uint8_t)E_INK_WIDTH);
    sendData((uint8_t)(E_INK_HEIGHT >> 8));
    sendData((uint8_t)(E_INK_HEIGHT & 0xff));

    sendCommand(0x50); // VCOM and data interval
    sendData((uint8_t)0x77);

    return true;
  } else {
    // Sleep
    sendCommand(0x50);
    sendData((uint8_t)0xf7);

    sendCommand(0x02); // Power off
    waitForEpd(BUSY_TIMEOUT_MS);

    sendCommand(0x07); // Deep sleep
    sendData((uint8_t)0xA5);

    vTaskDelay(pdMS_TO_TICKS(1));

    gpio_set_direction(EPAPER_BUSY_PIN, GPIO_MODE_INPUT);

    m_spi.deinit();

    return true;
  }
}
