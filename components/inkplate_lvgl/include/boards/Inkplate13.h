/**
 * @file Inkplate13.h
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

#pragma once

#include "soc/gpio_reg.h"
#include "soc/gpio_struct.h"

#include "BoardCommon.h"
#include "GraphicsDefs.h"

#include "PCAL.h"
#include "RTC.h"
#include "SPI.h"

#define IO_INT_ADDR 0x20

// Color display of the spectra panel
static uint32_t pallete[] = {0x000000, 0xFFFFFF, 0xFFFF00,
                             0xFF0000, 0x0000FF, 0x00FF00};

// Pin on the internal io expander which controls MOSFET for turning on and off
// the SD card
#define SD_PMOS_PIN IO_NUM_B2 // 10

// Pinout definition - should be hard-coded.
#define SPECTRA133_DC_PIN GPIO_NUM_14
#define SPECTRA133_CS_M_PIN GPIO_NUM_42
#define SPECTRA133_RST_PIN GPIO_NUM_4
#define SPECTRA133_BUSYN_PIN GPIO_NUM_7
#define SPECTRA133_SPI_MOSI GPIO_NUM_40
#define SPECTRA133_SPI_MISO GPIO_NUM_41
#define SPECTRA133_SPI_SCK GPIO_NUM_38
#define SPECTRA133_CS_S_PIN GPIO_NUM_39
#define SPECTRA133_PWR_EN GPIO_NUM_21
#define SPECTRA133_BS0 GPIO_NUM_6
#define SPECTRA133_BS1 GPIO_NUM_5

// Define the default Spectra6 color indexes.
#define INKPLATE_BLACK 0
#define INKPLATE_WHITE 1
#define INKPLATE_YELLOW 2
#define INKPLATE_RED 3
#define INKPLATE_BLUE 5
#define INKPLATE_GREEN 6

static uint16_t colorPalette[6] = {INKPLATE_BLACK,  INKPLATE_WHITE,
                                   INKPLATE_YELLOW, INKPLATE_RED,
                                   INKPLATE_BLUE,   INKPLATE_GREEN};

// Screen resolution.
#define E_INK_WIDTH 1200
#define E_INK_HEIGHT 1600

// Register addresses
#define SPECTRA133_REGISTER_PSR 0x00
#define SPECTRA133_REGISTER_PWR 0x01
#define SPECTRA133_REGISTER_POF 0x02
#define SPECTRA133_REGISTER_PON 0x04
#define SPECTRA133_REGISTER_BTST_N 0x05
#define SPECTRA133_REGISTER_BTST_P 0x06
#define SPECTRA133_REGISTER_DTM 0x10
#define SPECTRA133_REGISTER_DRF 0x12
#define SPECTRA133_REGISTER_PLL 0x30
#define SPECTRA133_REGISTER_TSC 0x40
#define SPECTRA133_REGISTER_CDI 0x50
#define SPECTRA133_REGISTER_TCON 0x60
#define SPECTRA133_REGISTER_TRES 0x61
#define SPECTRA133_REGISTER_PTLW 0x83
#define SPECTRA133_REGISTER_AN_TM 0x74
#define SPECTRA133_REGISTER_AGID 0x86
#define SPECTRA133_REGISTER_BUCK_BOOST_VDDN 0xB0
#define SPECTRA133_REGISTER_TFT_VCOM_POWER 0xB1
#define SPECTRA133_REGISTER_EN_BUF 0xB6
#define SPECTRA133_REGISTER_BOOST_VDDP_EN 0xB7
#define SPECTRA133_REGISTER_CCSET 0xE0
#define SPECTRA133_REGISTER_PWS 0xE3
#define SPECTRA133_REGISTER_CMD66 0xF0

// Register Values
static uint8_t SPECTRA133_REGISTER_PSR_V[2] = {0xDF, 0x6B};
static uint8_t SPECTRA133_REGISTER_PWR_V[6] = {0x0F, 0x00, 0x28,
                                               0x2C, 0x28, 0x38};
static uint8_t SPECTRA133_REGISTER_POF_V[1] = {0x00};
static uint8_t SPECTRA133_REGISTER_DRF_V[1] = {0x00};
static uint8_t SPECTRA133_REGISTER_PLL_V[1] = {0x08};
static uint8_t SPECTRA133_REGISTER_CDI_V[1] = {0xF7};
static uint8_t SPECTRA133_REGISTER_TCON_V[2] = {0x03, 0x03};
static uint8_t SPECTRA133_REGISTER_TRES_V[4] = {0x04, 0xB0, 0x03, 0x20};
static uint8_t SPECTRA133_REGISTER_CMD66_V[6] = {0x49, 0x55, 0x13,
                                                 0x5D, 0x05, 0x10};
static uint8_t SPECTRA133_REGISTER_EN_BUF_V[1] = {0x07};
static uint8_t SPECTRA133_REGISTER_CCSET_V[1] = {0x01};
static uint8_t SPECTRA133_REGISTER_PWS_V[1] = {0x22};
static uint8_t SPECTRA133_REGISTER_AN_TM_V[9] = {0xC0, 0x1C, 0x1C, 0xCC, 0xCC,
                                                 0xCC, 0x15, 0x15, 0x55};
static uint8_t SPECTRA133_REGISTER_AGID_V[1] = {0x10};
static uint8_t SPECTRA133_REGISTER_BTST_P_V[2] = {0xD8, 0x18};
static uint8_t SPECTRA133_REGISTER_BOOST_VDDP_EN_V[1] = {0x01};
static uint8_t SPECTRA133_REGISTER_BTST_N_V[2] = {0xD8, 0x18};
static uint8_t SPECTRA133_REGISTER_BUCK_BOOST_VDDN_V[1] = {0x01};
static uint8_t SPECTRA133_REGISTER_TFT_VCOM_POWER_V[1] = {0x02};

/**
 * @brief Class for Inkplate 13.
 *
 */
class Inkplate13 : public BoardCommon {
public:
  /**
   * @brief Construct a new Inkplate 13 object.
   *
   */
  Inkplate13();

  // not used
  uint32_t partialUpdate(bool forced = false, bool leaveOn = false) {
    return 0;
  };
  esp_err_t einkOn() { return ESP_OK };
  esp_err_t einkOff() { return ESP_OK };

  RTC rtc;

private:
  enum eSpectraChipID {
    eChipIdMaster = 1,
    eChipIdSlave = 2,
    eChipIdBoth = eChipIdMaster | eChipIdSlave
  };

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
   * @param chipId chip to send to (master, slave or both).
   */
  void sendCommandData(uint8_t cmd, uint8_t *data, int n,
                       enum eSpectraChipID chipId);

  // not used
  void calculateLUTs() { return; };
  esp_err_t display1b(bool leaveOn) { return ESP_OK; };
  void gpioInit() { return; };
  void clean(uint8_t c, uint8_t rep) { return; };
  void pinsAsOutputs() { return; };
  void pinsZstate() { return; };

  SPI m_spi;
};

#endif
