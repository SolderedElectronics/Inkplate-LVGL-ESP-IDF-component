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
#include "string.h"
#include "lvgl.h"

static const char *TAG = "Inkplate2";

/* -------------------------------------------------------------------------- */
/*                              Public functions                              */
/* -------------------------------------------------------------------------- */

Inkplate2::Inkplate2() : m_spi(EPAPER_DIN, EPAPER_CLK) {
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

  lv_color_format_t fmt = lv_display_get_color_format(disp);
  uint8_t bpp = lv_color_format_get_size(fmt);

  for (int32_t y = 0; y < h; y++) {
    const uint8_t *row = px_map + (size_t)y * w * bpp;
    for (int32_t x = 0; x < w; x++) {
      const uint8_t *p = row + x * bpp;
      uint8_t r, g, b;

      if (fmt == LV_COLOR_FORMAT_RGB565) {
        uint16_t px = p[0] | ((uint16_t)p[1] << 8);
        r = ((px >> 11) & 0x1F) << 3;
        g = ((px >> 5) & 0x3F) << 2;
        b = (px & 0x1F) << 3;
      } else if (fmt == LV_COLOR_FORMAT_RGB888) {
        b = p[0]; g = p[1]; r = p[2]; // lv_color_t layout: blue, green, red
      } else {
        r = g = b = p[0]; // L8 or unknown: single byte luminance
      }

      uint16_t bright = (uint16_t)r + g + b;
      uint8_t color;
      if (r > 160 && g < 128 && b < 128) {
        color = INKPLATE2_RED;
      } else if (bright < 280) {
        color = INKPLATE2_BLACK;
      } else {
        color = INKPLATE2_WHITE;
      }

      self->writePixelInternal(area->x1 + x, area->y1 + y, color);
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
