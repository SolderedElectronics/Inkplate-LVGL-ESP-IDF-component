/**
 * @file BoardCommon.h
 * @author Fran Fodor for Soldered
 * @brief Shared functions for inkplates.
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

#include "BoardBase.h"
#include "esp_err.h"
#include "esp_rom_sys.h"
#include <stdint.h>

#if defined(CONFIG_INKPLATE_BOARD_INKPLATE10) ||                               \
    defined(CONFIG_INKPLATE_BOARD_INKPLATE6) ||                                \
    defined(CONFIG_INKPLATE_BOARD_INKPLATE6FLICK) ||                           \
    defined(CONFIG_INKPLATE_BOARD_INKPLATE5) ||                                \
    defined(CONFIG_INKPLATE_BOARD_INKPLATE4)
// enables the GPIO0/CL level shifter
#define GPIO0_ENABLE IO_NUM_B0

#define WAKEUP IO_NUM_A3
#define WAKEUP_SET                                                             \
  do {                                                                         \
    expander1.setLevel(WAKEUP, 1, true);                                       \
  } while (0)
#define WAKEUP_CLEAR                                                           \
  do {                                                                         \
    expander1.setLevel(WAKEUP, 0, true);                                       \
  } while (0)

#define PWRUP IO_NUM_A4
#define PWRUP_SET                                                              \
  do {                                                                         \
    expander1.setLevel(PWRUP, 1, true);                                        \
  } while (0)
#define PWRUP_CLEAR                                                            \
  do {                                                                         \
    expander1.setLevel(PWRUP, 0, true);                                        \
  } while (0)

#define VCOM IO_NUM_A5
#define VCOM_SET                                                               \
  do {                                                                         \
    expander1.setLevel(VCOM, 1, true);                                         \
  } while (0)
#define VCOM_CLEAR                                                             \
  do {                                                                         \
    expander1.setLevel(VCOM, 0, true);                                         \
  } while (0)

#define OE IO_NUM_A0
#define OE_SET                                                                 \
  do {                                                                         \
    expander1.setLevel(OE, 1, true);                                           \
  } while (0)
#define OE_CLEAR                                                               \
  do {                                                                         \
    expander1.setLevel(OE, 0, true);                                           \
  } while (0)

#define GMOD IO_NUM_A1
#define GMOD_SET                                                               \
  do {                                                                         \
    expander1.setLevel(GMOD, 1, true);                                         \
  } while (0)
#define GMOD_CLEAR                                                             \
  do {                                                                         \
    expander1.setLevel(GMOD, 0, true);                                         \
  } while (0)

#define SPV IO_NUM_A2
#define SPV_SET                                                                \
  do {                                                                         \
    expander1.setLevel(SPV, 1, true);                                          \
  } while (0)
#define SPV_CLEAR                                                              \
  do {                                                                         \
    expander1.setLevel(SPV, 0, true);                                          \
  } while (0)

#define CL 0x01
#define CL_SET                                                                 \
  do {                                                                         \
    GPIO.out_w1ts = CL;                                                        \
  } while (0)
#define CL_CLEAR                                                               \
  do {                                                                         \
    GPIO.out_w1tc = CL;                                                        \
  } while (0)

#define CKV 0x01
#define CKV_SET                                                                \
  do {                                                                         \
    GPIO.out1_w1ts.val = CKV;                                                  \
  } while (0)
#define CKV_CLEAR                                                              \
  do {                                                                         \
    GPIO.out1_w1tc.val = CKV;                                                  \
  } while (0)

#define SPH 0x02
#define SPH_SET                                                                \
  do {                                                                         \
    GPIO.out1_w1ts.val = SPH;                                                  \
  } while (0)
#define SPH_CLEAR                                                              \
  do {                                                                         \
    GPIO.out1_w1tc.val = SPH;                                                  \
  } while (0)

#define LE 0x04
#define LE_SET                                                                 \
  do {                                                                         \
    GPIO.out_w1ts = LE;                                                        \
  } while (0)
#define LE_CLEAR                                                               \
  do {                                                                         \
    GPIO.out_w1tc = LE;                                                        \
  } while (0)
#endif

/**
 * @brief Class for common board functions.
 *
 */
class BoardCommon : public BoardBase {
public:
  /**
   * @brief Construct a new Board Common object.
   *
   * @param einkWidth panel width in pixels.
   * @param einkHeight panel height in pixels.
   * @param cleanCycles1 number of full-white cleaning passes in cleanBurnIn().
   * @param cleanCycles0 number of full-black cleaning passes in cleanBurnIn().
   */
  BoardCommon(uint16_t einkWidth, uint16_t einkHeight, uint8_t cleanCycles1,
              uint8_t cleanCycles0);

  /**
   * @brief Select the active display mode (black-and-white or grayscale).
   *
   * @param mode BLACK_AND_WHITE or GRAYSCALE.
   */
  void setDisplayMode(displayMode_t mode);

  /**
   * @brief Get the display mode.
   *
   * @return displayMode_t BLACK_AND_WHITE or GRAYSCALE.
   */
  displayMode_t getDisplayMode() { return m_displayMode; }

  /**
   * @brief Fill the framebuffer with white (erase all content).
   */
  void clearDisplay();

  /**
   * @brief Fill the framebuffer with black (all pixels on).
   */
  void fillDisplay();

  /**
   * @brief Copy data from partial to data buffer.
   *
   */
  void preloadScreen();

  /**
   * @brief Write a single pixel into the framebuffer after applying display
   * rotation.
   *
   * @param x logical x coordinate.
   * @param y logical y coordinate.
   * @param color pixel value (0–7 for grayscale; 0 or 1 for B&W).
   */
  void writePixelInternal(int16_t x, int16_t y, uint16_t color);

  /**
   * @brief Push the framebuffer to the e-ink panel.
   *
   * @param leaveOn if true, leave the panel powered on after the update.
   * @return esp_err_t error code.
   */
  esp_err_t display(bool leaveOn = false);

  /**
   * @brief Mark all PMIC control pins on expander1 as blocked so user code
   *        cannot accidentally modify them via the PCAL API.
   *
   */
  void blockGpioPins();

  /**
   * @brief Set the number of partial updates allowed before a forced full
   * refresh.
   *
   * @param numberOfPartialUpdates maximum consecutive partial updates; 0
   * disables the limit.
   */
  void setFullUpdateThreshold(uint16_t numberOfPartialUpdates);

  /**
   * @brief run multiple full-panel cleaning cycles to reduce burn-in.
   *
   * @param cleanCycles number of cleaning iterations.
   * @param cleanDelay delay in milliseconds between iterations.
   */
  void cleanBurnIn(uint8_t cleanCycles, uint16_t cleanDelay);

  /**
   * @brief Read the LiPo battery voltage via the ESP32 ADC.
   *
   * @return double battery voltage in volts (approximately 3.0–4.2 V when
   * charged).
   */
  double readBattery();

  /**
   * @brief Program a new VCOM voltage into the TPS65186 and persist it in NVS.
   *
   * @param vcom VCOM in volts; must be in the range -5.0 to 0.0.
   * @return esp_err_t error code.
   */
  esp_err_t setVCOM(double vcom);

  /**
   * @brief Read the VCOM voltage stored in NVS (written by setVCOM()).
   *
   * @return double stored VCOM in volts, or 0.0 if no value has been saved.
   */
  double getVCOM();

  /**
   * @brief Read the VCOM currently programmed into the TPS65186 hardware
   * registers.
   *
   * @return double live VCOM in volts, or 0.0 on error
   *
   * @note Briefly powers the panel on to read the TPS registers.
   */
  double getStoredVCOM();

  /**
   * @brief Read the e-ink panel temperature from the TPS65186 thermistor.
   *
   * @return int8_t temperature in degrees Celsius, or 0 if the panel could not
   * be powered on.
   *
   * @note Briefly powers the panel on to communicate with the TPS65186.
   */
  int8_t readTemperature();

  /**
   * @brief Mount the SD card and make it accessible via the VFS.
   *
   * @return esp_err_t sd card error code.
   */
  esp_err_t sdCardInit();

  /**
   * @brief Put the SD card into low-power sleep mode.
   *
   * @return esp_err_t sd card error code.
   */
  esp_err_t sdCardSleep();

  /**
   * @brief Mount the SD card and make it accessible via the VFS.
   *
   * @return const char* null-terminated mount point string.
   */
  const char *getMountPoint();

protected:
  // board-specific display drivers — must be implemented per board
  virtual esp_err_t display1b(bool leaveOn) = 0;
  virtual esp_err_t display3b(bool leaveOn) = 0;
  virtual uint32_t partialUpdate(bool forced, bool leaveOn) = 0;
  virtual void clean(uint8_t c, uint8_t rep) = 0;
  virtual void gpioInit() = 0;
  virtual void pinsAsOutputs() = 0;
  virtual void pinsZstate() = 0;
  virtual esp_err_t initBuffers() = 0;
  virtual void calculateLUTs() = 0;

  // shared low-level helpers

  /**
   * @brief Assert the vertical scan start pulse (SPV strobed via CKV).
   *
   * @note Timing values derived from ED060SC4 panel datasheet.
   */
  void vscanStart();

  /**
   * @brief Latch the current scan line into the panel (CKV low, LE pulse).
   *
   */
  void vscanEnd();

  /**
   * @brief Set the cached panel power state.
   *
   * @param state true = panel on, false = panel off.
   */
  void setPanelState(bool state);

  /**
   * @brief Return the cached panel power state.
   *
   * @return bool true = panel on, false = panel off.
   */
  bool getPanelState();

  /**
   * @brief Initialise the TPS65186 PMIC with the default power sequences.
   *
   * @return esp_err_t i2c error code.
   */
  esp_err_t pmicBegin();

  uint16_t m_einkWidth;
  uint16_t m_einkHeight;
  uint8_t m_cleanCycles1;
  uint8_t m_cleanCycles0;

  displayMode_t m_displayMode = GRAYSCALE;

  uint8_t *m_framebufferColor = nullptr;
  uint8_t *m_framebuffer = nullptr;
  uint8_t *m_newFramebuffer = nullptr;
  uint8_t *m_waveformBuffer = nullptr;

  uint16_t m_partialUpdateLimiter = 10;
  uint16_t m_partialUpdateCounter = 0;
  bool m_blockPartial = true;
  bool m_panelState = false;
};