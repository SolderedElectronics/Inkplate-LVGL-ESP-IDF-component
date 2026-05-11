/**
 * @file Inkplate6Color.h
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

#pragma once

#include "esp_err.h"
#include "esp_rom_sys.h"
#include "soc/gpio_reg.h"
#include "soc/gpio_struct.h"

#include "BoardCommon.h"
#include "GraphicsDefs.h"

#include "PCAL.h"
#include "RTC.h"
#include "SPI.h"

#define IO_INT_ADDR 0x20

extern PCAL expander1;

// Pin on the internal io expander which controls MOSFET for turning on and off
// the SD card
#define SD_PMOS_PIN IO_NUM_B2 // 10

// Connections between ESP32 and color Epaper
#define EPAPER_RST_PIN GPIO_NUM_19
#define EPAPER_DC_PIN GPIO_NUM_33
#define EPAPER_CS_PIN GPIO_NUM_27
#define EPAPER_BUSY_PIN GPIO_NUM_32
#define EPAPER_CLK GPIO_NUM_18
#define EPAPER_DIN GPIO_NUM_23

// Timeout for init of epaper (1.5 sec in this case)
#define INIT_TIMEOUT 1500

// Epaper registers
#define PANEL_SET_REGISTER 0x00
#define POWER_SET_REGISTER 0x01
#define POWER_OFF_SEQ_SET_REGISTER 0x03
#define POWER_OFF_REGISTER 0x04
#define BOOSTER_SOFTSTART_REGISTER 0x06
#define DEEP_SLEEP_REGISTER 0x07
#define DATA_START_TRANS_REGISTER 0x10
#define DATA_STOP_REGISTER 0x11
#define DISPLAY_REF_REGISTER 0x12
#define IMAGE_PROCESS_REGISTER 0x13
#define PLL_CONTROL_REGISTER 0x30
#define TEMP_SENSOR_REGISTER 0x40
#define TEMP_SENSOR_EN_REGISTER 0x41
#define TEMP_SENSOR_WR_REGISTER 0x42
#define TEMP_SENSOR_RD_REGISTER 0x43
#define VCOM_DATA_INTERVAL_REGISTER 0x50
#define LOW_POWER_DETECT_REGISTER 0x51
#define RESOLUTION_SET_REGISTER 0x61
#define STATUS_REGISTER 0x71
#define VCOM_VALUE_REGISTER 0x81
#define VCM_DC_SET_REGISTER 0x02

// Epaper resolution and colors
#define E_INK_WIDTH 600
#define E_INK_HEIGHT 448
#define INKPLATE_BLACK 0b00000000
#define INKPLATE_WHITE 0b00000001
#define INKPLATE_GREEN 0b00000010
#define INKPLATE_BLUE 0b00000011
#define INKPLATE_RED 0b00000100
#define INKPLATE_YELLOW 0b00000101
#define INKPLATE_ORANGE 0b00000110

/**
 * @brief Class for Inkplate 6 Color.
 *
 */
class Inkplate6Color : public BoardCommon {
public:
  /**
   * @brief Construct a new Inkplate 6 Color object.
   *
   */
  Inkplate6Color();

  /**
   * @brief Inkplate 6 Color does not support partial updates.
   */
  uint32_t partialUpdate(bool forced = false, bool leaveOn = false) {
    return 0;
  };

  /**
   * @brief Power is managed internally per display() call.
   *
   * @return esp_err_t ESP_OK
   */
  esp_err_t einkOn() { return ESP_OK; };

  /**
   * @brief Power is managed internally per display() call.
   *
   * @return esp_err_t ESP_OK
   */
  esp_err_t einkOff() { return ESP_OK; };

  RTC rtc;

private:
  /**
   * @brief Allocate framebuffers.
   *
   * @return esp_err_t error code.
   */
  esp_err_t initBuffers();

  /**
   * @brief Drive the panel using 3-bit (8-level grayscale) waveform.
   *
   * @param leaveOn if true, leave the panel powered on after the update.
   * @return esp_err_t error code
   */
  esp_err_t display3b(bool leaveOn);

  /**
   * @brief Polls the busy pin until the panel is ready or the timeout expires.
   *
   * @param timeout maximum wait time in milliseconds.
   * @return bool true if the panel became ready, false on timeout.
   */
  bool waitForEpd(uint32_t timeout);

  /**
   * @brief Issues a hardware reset pulse to the panel.
   *
   */
  void resetPanel();

  /**
   * @brief Sends a single command byte to the panel.
   *
   * @param command command byte to send.
   */
  void sendCommand(uint8_t command);

  /**
   * @brief Sends a data buffer to the panel in 4092-byte chunks.
   *
   * @param data pointer to the data buffer.
   * @param n number of bytes to send.
   */
  void sendData(uint8_t *data, int n);

  /**
   * @brief Sends a single data byte to the panel.
   *
   * @param data byte to send.
   */
  void sendData(uint8_t data);

  /**
   * @brief Wakes or puts the panel into deep sleep.
   *
   * @param state true to enter deep sleep, false to wake.
   * @return bool true on success, false if the busy timeout expired during
   * wake.
   */
  bool setPanelDeepSleep(bool state);

  /**
   * @brief Turns off pins to save power.
   *
   */
  void setIOExpanderForLowPower();

  // not used
  void calculateLUTs() { return; };
  esp_err_t display1b(bool leaveOn) { return ESP_OK; };
  void gpioInit() { return; };
  void clean(uint8_t c, uint8_t rep) { return; };
  void pinsAsOutputs() { return; };
  void pinsZstate() { return; };

  SPI m_spi;
};