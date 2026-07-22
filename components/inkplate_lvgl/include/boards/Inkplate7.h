/**
 * @file Inkplate7.h
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

#pragma once

#include "soc/gpio_reg.h"
#include "soc/gpio_struct.h"

#include "BoardCommon.h"
#include "GraphicsDefs.h"
#include "ditherAlgorithm.h"
#include "lvgl.h"

#include "PCAL.h"
#include "RTC.h"
#include "SPI.h"

void display_flush_callback_inkplate7(lv_display_t *disp,
                                      const lv_area_t *area, uint8_t *px_map);

#define IO_INT_ADDR 0x20

// Pin on the internal io expander which controls MOSFET for turning on and
// off the SD card
#define SD_PMOS_PIN IO_NUM_B2 // 10

// Pinout definition - ported from the Arduino Spectra7 driver (pins.h).
#define SPECTRA73_DC_PIN GPIO_NUM_14
#define SPECTRA73_CS_PIN GPIO_NUM_42
#define SPECTRA73_RST_PIN GPIO_NUM_4
#define SPECTRA73_BUSYN_PIN GPIO_NUM_7
#define SPECTRA73_SPI_MOSI GPIO_NUM_40
#define SPECTRA73_SPI_SCK GPIO_NUM_38
#define SPECTRA73_PWR_EN GPIO_NUM_21
#define SPECTRA73_BS0 GPIO_NUM_6
#define SPECTRA73_BS1 GPIO_NUM_5

// Battery measurement pins.
#define SPECTRA73_BATT_MOSFET_PIN IO_NUM_B1 // pin on the internal io expander
#define SPECTRA73_BATT_ADC_PIN GPIO_NUM_1

// Define the default Spectra7 color indexes (as declared in the Arduino
// source's pins.h).
#define INKPLATE_BLACK 0
#define INKPLATE_WHITE 1
#define INKPLATE_YELLOW 2
#define INKPLATE_RED 3
#define INKPLATE_BLUE 4
#define INKPLATE_GREEN 5

// NOTE: same as the ESP-IDF (non-LVGL) Inkplate7 port - the Arduino source's
// colorPalette array uses literal 5/6 for blue/green (hardware color codes
// that skip index 4, same scheme as Spectra6/Inkplate13), not the
// INKPLATE_BLUE/INKPLATE_GREEN macros above. paletteIndices below carries
// that remap into the LVGL dithering pipeline. Flagged for confirmation.
static uint16_t palette7[6] = {
    0x0000, // black
    0xFFFF, // white
    0xFFE0, // yellow
    0xF800, // red
    0x001F, // blue
    0x07E0, // green
};
static uint8_t paletteIndices7[6] = {INKPLATE_BLACK, INKPLATE_WHITE,
                                     INKPLATE_YELLOW, INKPLATE_RED, 5, 6};

// Screen resolution.
#define E_INK_WIDTH 800
#define E_INK_HEIGHT 480

// Register addresses
#define SPECTRA73_REGISTER_PSR 0x00
#define SPECTRA73_REGISTER_PWR 0x01
#define SPECTRA73_REGISTER_POF 0x02
#define SPECTRA73_REGISTER_PFS 0x03
#define SPECTRA73_REGISTER_PON 0x04
#define SPECTRA73_REGISTER_BTST1 0x05
#define SPECTRA73_REGISTER_BTST2 0x06
#define SPECTRA73_REGISTER_DSLP 0x07
#define SPECTRA73_REGISTER_BTST3 0x08
#define SPECTRA73_REGISTER_DTM 0x10
#define SPECTRA73_REGISTER_DRF 0x12
#define SPECTRA73_REGISTER_IPC 0x13
#define SPECTRA73_REGISTER_PLL 0x30
#define SPECTRA73_REGISTER_TSE 0x41
#define SPECTRA73_REGISTER_CDI 0x50
#define SPECTRA73_REGISTER_TCON 0x60
#define SPECTRA73_REGISTER_TRES 0x61
#define SPECTRA73_REGISTER_VDCS 0x82
#define SPECTRA73_REGISTER_T_VDCS 0x84
#define SPECTRA73_REGISTER_AGID 0x86
#define SPECTRA73_REGISTER_CCSET 0xE0
#define SPECTRA73_REGISTER_PWS 0xE3
#define SPECTRA73_REGISTER_TSSET 0xE6
#define SPECTRA73_REGISTER_CMDH 0xAA

// Register values
static uint8_t SPECTRA73_REGISTER_CMDH_V[6] = {0x49, 0x55, 0x20,
                                               0x08, 0x09, 0x18};
static uint8_t SPECTRA73_REGISTER_PWR_V[6] = {0x3F, 0x00, 0x32,
                                              0x2A, 0x0E, 0x2A};
static uint8_t SPECTRA73_REGISTER_PSR_V[2] = {0x5F, 0x69};
static uint8_t SPECTRA73_REGISTER_PFS_V[4] = {0x00, 0x54, 0x00, 0x44};
static uint8_t SPECTRA73_REGISTER_BTST1_V[4] = {0x40, 0x1F, 0x1F, 0x2C};
static uint8_t SPECTRA73_REGISTER_BTST2_V[4] = {0x6F, 0x1F, 0x16, 0x25};
static uint8_t SPECTRA73_REGISTER_BTST3_V[4] = {0x6F, 0x1F, 0x1F, 0x22};
static uint8_t SPECTRA73_REGISTER_IPC_V[2] = {0x00, 0x04};
static uint8_t SPECTRA73_REGISTER_PLL_V[1] = {0x02};
static uint8_t SPECTRA73_REGISTER_TSE_V[1] = {0x00};
static uint8_t SPECTRA73_REGISTER_CDI_V[1] = {0x3F};
static uint8_t SPECTRA73_REGISTER_TCON_V[2] = {0x02, 0x00};
static uint8_t SPECTRA73_REGISTER_TRES_V[4] = {0x03, 0x20, 0x01, 0xE0};
static uint8_t SPECTRA73_REGISTER_VDCS_V[1] = {0x1E};
static uint8_t SPECTRA73_REGISTER_T_VDCS_V[1] = {0x01};
static uint8_t SPECTRA73_REGISTER_AGID_V[1] = {0x00};
static uint8_t SPECTRA73_REGISTER_PWS_V[1] = {0x2F};
static uint8_t SPECTRA73_REGISTER_CCSET_V[1] = {0x00};
static uint8_t SPECTRA73_REGISTER_TSSET_V[1] = {0x00};
static uint8_t SPECTRA73_REGISTER_POF_V[1] = {0x00};
static uint8_t SPECTRA73_REGISTER_DRF_V[1] = {0x00};

/**
 * @brief Class for Inkplate 7.
 *
 */
class Inkplate7 : public BoardCommon {
public:
  /**
   * @brief Construct a new Inkplate 7 object.
   *
   */
  Inkplate7(lv_display_render_mode_t mode = LV_DISPLAY_RENDER_MODE_FULL);

  lv_display_t *getDisplay() { return m_disp; }

  void enableDithering(bool state) { m_ditherEnabled = state; }
  bool m_ditherEnabled = false;
  DitherAlgorithm m_dither;

  /**
   * @brief Inkplate 7 does not support partial updates.
   */
  uint32_t partialUpdate(bool forced = false, bool leaveOn = false) {
    return 0;
  };

  /**
   * @brief Power is managed internally per display() call.
   *
   * @return esp_err_t ESP_OK
   */
  esp_err_t einkOn() { return ESP_OK; }

  /**
   * @brief Power is managed internally per display() call.
   *
   * @return esp_err_t ESP_OK
   */
  esp_err_t einkOff() { return ESP_OK; }

  /**
   * @brief Read the battery voltage. Overrides BoardCommon::readBattery()
   * since this board's battery MOSFET pin and ADC pin/channel differ from
   * the generic implementation, and it additionally auto-detects PMOS-only
   * vs PMOS+NMOS board revisions.
   *
   * @return double battery voltage in volts.
   */
  double readBattery();

  /**
   * @brief Fill the framebuffer with white (erase all content).
   *
   * @note Overrides BoardCommon::clearDisplay(): the Arduino source fills
   * with the white palette index (0x11) rather than BoardCommon's generic
   * 0xFF fill.
   */
  void clearDisplay();

  /**
   * @brief Fill the framebuffer with black (all pixels on).
   */
  void fillDisplay();

  RTC rtc;

private:
  /**
   * @brief Allocate framebuffers.
   *
   * @return esp_err_t error code.
   */
  esp_err_t initBuffers();

  /**
   * @brief Drive the panel using the color waveform.
   *
   * @param leaveOn if true, leave the panel powered on after the update.
   * @return esp_err_t error code
   */
  esp_err_t display3b(bool leaveOn);

  /**
   * @brief Polls the busy pin until the panel is ready or the timeout
   * expires.
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
   * @brief Configures GPIOs and starts SPI communication with the panel.
   *
   */
  void setIO();

  /**
   * @brief Powers the panel on (initializing it) or off, skipping the
   * transition if the panel is already in the requested state.
   *
   * @param state true to power on, false to power off.
   */
  void setPanelPower(bool state);

  /**
   * @brief Turns off pins to save power / drain residual charge.
   *
   */
  void setPanelPinsToLow();

  /**
   * @brief Initializes the screen.
   *
   */
  void screenInit();

  /**
   * @brief Sends command and data.
   *
   * @param cmd command to send.
   * @param data pointer to the data buffer.
   * @param n number of bytes to send.
   */
  void sendCommand(uint8_t cmd, const uint8_t *data, int n);

  // not used
  void calculateLUTs() { return; };
  esp_err_t display1b(bool leaveOn) { return ESP_OK; };
  void gpioInit() { return; };
  void clean(uint8_t c, uint8_t rep) { return; };
  void pinsAsOutputs() { return; };
  void pinsZstate() { return; };

  SPI m_spi;

  uint8_t *m_lvglBuf = nullptr;
  lv_display_t *m_disp = nullptr;

  friend void display_flush_callback_inkplate7(lv_display_t *disp,
                                               const lv_area_t *area,
                                               uint8_t *px_map);
};
