/**
 * @file TouchCypress.h
 * @author Fran Fodor for Soldered
 * @brief Driver for Cypress touchscreen.
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

#include "PCAL.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "esp_attr.h"
#include "esp_timer.h"
#include <string.h>

#include "Touch.h"

#define TOUCHSCREEN_I2C_ADDR 0x24

#define CYPRESS_TOUCH_BASE_ADDR 0x00
#define CYPRESS_TOUCH_SOFT_RST_MODE 0x01
#define CYPRESS_TOUCH_SYSINFO_MODE 0x10
#define CYPRESS_TOUCH_OPERATE_MODE 0x00
#define CYPRESS_TOUCH_LOW_POWER_MODE 0x04
#define CYPRESS_TOUCH_DEEP_SLEEP_MODE 0x02
#define CYPRESS_TOUCH_REG_ACT_INTRVL 0x1D

#define TOUCHSCREEN_EN IO_NUM_B4
#define TOUCHSCREEN_RST IO_NUM_B2
#define TOUCHSCREEN_INT GPIO_NUM_36
#define TOUCHSCREEN_IO_EXPANDER IO_INT_ADDR
#define TOUCHSCREEN_IO_REGS ioRegsInt

#define CYPRESS_TOUCH_ACT_INTRVL_DFLT 0x00
#define CYPRESS_TOUCH_LP_INTRVL_DFLT 0x0A
#define CYPRESS_TOUCH_TCH_TMOUT_DFLT 0xFF

#define CYPRESS_TOUCH_MAX_X 682
#define CYPRESS_TOUCH_MAX_Y 1023

#define E_INK_WIDTH 1024
#define E_INK_HEIGHT 758

/**
 * @brief  Bootloader register snapshot from the Cypress touch controller.
 *
 * Read during initialisation to verify the controller firmware version
 * and to determine whether the controller is still in bootloader mode.
 */
struct cyttspBootloaderData {
  uint8_t bl_file;
  uint8_t bl_status;
  uint8_t bl_error;
  uint8_t blver_hi;
  uint8_t blver_lo;
  uint8_t bld_blver_hi;
  uint8_t bld_blver_lo;
  uint8_t ttspver_hi;
  uint8_t ttspver_lo;
  uint8_t appid_hi;
  uint8_t appid_lo;
  uint8_t appver_hi;
  uint8_t appver_lo;
  uint8_t cid_0;
  uint8_t cid_1;
  uint8_t cid_2;
};

/**
 * @brief  System info register snapshot from the Cypress touch controller.
 *
 * Read and written during initialisation to configure scan timing.
 */
struct cyttspSysinfoData {
  uint8_t hst_mode;
  uint8_t mfg_stat;
  uint8_t mfg_cmd;
  uint8_t cid[3];
  uint8_t tt_undef1;
  uint8_t uid[8];
  uint8_t bl_verh;
  uint8_t bl_verl;
  uint8_t tts_verh;
  uint8_t tts_verl;
  uint8_t app_idh;
  uint8_t app_idl;
  uint8_t app_verh;
  uint8_t app_verl;
  uint8_t tt_undef[5];
  uint8_t scn_typ;
  uint8_t act_intrvl;
  uint8_t tch_tmout;
  uint8_t lp_intrvl;
};

/**
 * @brief  Parsed touch report for up to two simultaneous touch points.
 */
struct cypressTouchData {
  uint8_t fingers;
  uint16_t x[2];
  uint16_t y[2];
  uint8_t z[2];
  uint8_t detectionType;
};

/**
 * @brief  Cypress CY8CTMA140 implementation of the Touch interface.
 *
 */
class TouchCypress : public Touch {
public:
  /**
   * @brief Initialises the touch controller.
   *
   * Registers the I2C device, powers on the controller, exits bootloader
   * mode, configures system info registers, and installs the GPIO interrupt.
   *
   * @param i2c I2C bus instance to register the device on.
   * @param expander IO expander used to control power and reset pins.
   * @param powerState non-zero to power on and initialise, zero to power off.
   *
   * @return ESP_OK on success, ESP_FAIL if the controller did not respond
   */
  esp_err_t begin(I2C &i2c, PCAL &expander, uint8_t powerState) override;

  /**
   * @brief  Powers off the touchscreen controller.
   */
  void shutdown() override;

  /**
   * @brief  Returns true if a new touch interrupt is pending.
   *
   * @return true if unread touch data is available.
   */
  bool available() override;

  /**
   * @brief Checks whether any active touch point falls within a given area.
   *
   * Reads pending touch data and caches it for 150 ms. Returns true if
   * one or both touch points land within the specified rectangle.
   *
   * @param x1 x coordinate of the top-left corner.
   * @param y1 y coordinate of the top-left corner.
   * @param w width of the area in pixels.
   * @param h height of the area in pixels.
   *
   * @return true if a touch point is within the area, false otherwise.
   */
  bool touchInArea(int16_t x1, int16_t y1, int16_t w, int16_t h) override;

  /**
   * @brief Reads touch coordinates for up to two fingers.
   *
   * Only returns data if the interrupt flag is set. Scales raw
   * coordinates to display resolution before returning.
   *
   * @param xPos array of at least 2 x coordinates.
   * @param yPos output array of at least 2 y coordinates.
   * @param z optional output array of at least 2 pressure values, pass NULL to
   * ignore.
   *
   * @return number of detected touch points (0, 1, or 2).
   */
  uint8_t getData(uint16_t *xPos, uint16_t *yPos, uint8_t *z = NULL) override;

  /**
   * @brief Reads 16 raw bytes from the base register into a buffer.
   *
   * @param b output buffer of at least 16 bytes.
   */
  void getRawData(uint8_t *b) override;

  /**
   * @brief Sets the controller power mode.
   *
   * @param state CYPRESS_TOUCH_OPERATE_MODE, CYPRESS_TOUCH_LOW_POWER_MODE, or
   * CYPRESS_TOUCH_DEEP_SLEEP_MODE.
   */
  void setPowerState(uint8_t state) override;

  /**
   * @brief  Reads the current power mode register from the controller.
   *
   * @return raw value of the base register.
   */
  uint8_t getPowerState() override;

private:
  /**
   * @brief Probes the controller over I2C to check if it is responsive.
   *
   * @param retries number of attempts before giving up.
   *
   * @return true if the controller acknowledged at least one attempt.
   */
  bool ping(int retries = 5);

  /**
   * @brief Performs an I2C handshake by toggling the MSB of the host mode
   * register.
   *
   * Must be called after reading touch data to acknowledge the interrupt
   * to the controller.
   */
  void handshake();

  /**
   * @brief Controls the touchscreen power supply and reset lines.
   *
   * @param power true to power on, false to power off.
   */
  void power(bool power);

  /**
   * @brief Reads raw touch data from the controller into a touch data struct.
   *
   * @param touchData output struct to populate.
   *
   * @return true on success, false on I2C error or null pointer.
   */
  bool getTouchData(struct cypressTouchData *touchData);

  /**
   * @brief Scales and optionally flips or swaps raw touch coordinates.
   *
   * Maps raw controller coordinates to display pixel coordinates.
   *
   * @param touchData touch data struct to modify in place.
   * @param xSize target x range (display width - 1).
   * @param ySize target y range (display height - 1).
   * @param flipX true to mirror along the x axis.
   * @param flipY true to mirror along the y axis.
   * @param swapXY true to swap x and y axes.
   */
  void scale(struct cypressTouchData *touchData, uint16_t xSize, uint16_t ySize,
             bool flipX, bool flipY, bool swapXY);

  /**
   * @brief Deinitialises the touchscreen and removes the interrupt handler.
   */
  void end();

  /**
   * @brief Issues a hardware reset pulse via the IO expander reset pin.
   */
  void reset();

  /**
   * @brief Issues a software reset command over I2C.
   */
  void swReset();

  /**
   * @brief Writes a single command byte to the base register.
   *
   * @param cmd  command byte to send.
   *
   * @return ESP_OK on success, or an I2C error code on failure.
   */
  esp_err_t sendCommand(uint8_t cmd);

  /**
   * @brief Reads the bootloader registers into a bootloader data struct.
   *
   * @param blDataPtr output struct to populate.
   *
   * @return ESP_OK on success.
   */
  esp_err_t loadBootloaderRegs(struct cyttspBootloaderData *blDataPtr);

  /**
   * @brief Reads a sequence of registers over I2C.
   *
   * @param cmd register address to read from.
   * @param buffer output buffer.
   * @param len number of bytes to read.
   *
   * @return ESP_OK on success, or an I2C error code on failure.
   */
  esp_err_t readI2CRegs(uint8_t cmd, uint8_t *buffer, int len);

  /**
   * @brief Writes a sequence of bytes to a register over I2C.
   *
   * @param cmd register address to write to.
   * @param buffer data to write.
   * @param len number of bytes to write.
   *
   * @return ESP_OK on success, ESP_FAIL on memory error, or an I2C error code.
   */
  esp_err_t writeI2CRegs(uint8_t cmd, uint8_t *buffer, int len);

  /**
   * @brief Sends the exit-bootloader command and verifies the controller left
   * bootloader mode.
   *
   * @return ESP_OK on success, ESP_FAIL if the controller remained in
   * bootloader mode.
   */
  esp_err_t exitBootloaderMode();

  /**
   * @brief Switches the controller to system info mode and reads the system
   * info registers.
   *
   * @param sysDataPtr output struct to populate.
   *
   * @return ESP_OK on success, ESP_FAIL if firmware version fields are zero.
   */
  esp_err_t setSysInfoMode(struct cyttspSysinfoData *sysDataPtr);

  /**
   * @brief Writes default timing values to the system info registers.
   *
   * @param sysDataPtr system info struct with values to write.
   *
   * @return ESP_OK on success, or an I2C error code on failure.
   */
  esp_err_t setSysInfoRegs(struct cyttspSysinfoData *sysDataPtr);

  PCAL *m_expander = nullptr;
  i2c_master_dev_handle_t m_devHandle = NULL;

  struct cyttspBootloaderData m_blData;
  struct cyttspSysinfoData m_sysData;

  uint8_t touchN;
  uint16_t touchX[2], touchY[2];
  uint32_t touchT = 0;
  bool m_tsInitDone = false;
};