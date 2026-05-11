/**
 * @file TouchElan.h
 * @author Fran Fodor for Soldered
 * @brief Driver for Elan touchscreen.
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

#include "driver/i2c_master.h"
#include "stdint.h"
#include "string.h"

#include "I2C.h"
#include "PCAL.h"
#include "Touch.h"

#define TOUCHSCREEN_EN IO_NUM_A0
#define TOUCHSCREEN_RST IO_NUM_A1
#define TOUCHSCREEN_INT GPIO_NUM_36
#define TOUCHSCREEN_I2C_ADDR 0x15

#define E_INK_WIDTH 600
#define E_INK_HEIGHT 600

#define BOUND(lo, val, hi) ((val) >= (lo) && (val) <= (hi))

/**
 * @brief  Elan implementation of the Touch interface.
 *
 */
class TouchElan : public Touch {
public:
  /**
   * @brief Initialises the touch controller.
   *
   * Registers the I2C device, installs the GPIO interrupt, performs
   * a hardware and software reset, and reads the sensor resolution.
   *
   * @param i2c I2C bus instance to register the device on.
   * @param expander IO expander used to control power and reset pins.
   * @param powerState initial power state passed to setPowerState().
   *
   * @return ESP_OK on success, ESP_FAIL if the controller did not respond.
   */
  esp_err_t begin(I2C &i2c, PCAL &expander, uint8_t powerState) override;

  /**
   * @brief  Powers off the touch controller and removes the interrupt handler.
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
   * Reads pending touch data and caches it for 100 ms. Returns true if
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
   * Applies rotation mapping before writing to the output arrays.
   *
   * @param xPos output array of at least 2 x coordinates.
   * @param yPos output array of at least 2 y coordinates.
   * @param z not used, kept for API.
   *
   * @return number of detected touch points (0, 1, or 2).
   */
  uint8_t getData(uint16_t *xPos, uint16_t *yPos,
                  uint8_t *z = nullptr) override;

  /**
   * @brief Reads 8 raw bytes from the touch controller into a buffer.
   *
   * @param b output buffer of at least 8 bytes.
   */
  void getRawData(uint8_t *b) override;

  /**
   * @brief Sets the controller power state.
   *
   * @param s 0 for active, 1 for low power; only bit 0 is used.
   */
  void setPowerState(uint8_t s) override;

  /**
   * @brief Reads the current power state from the controller.
   *
   * @return 0 for active, 1 for low power.
   */
  uint8_t getPowerState() override;

  /**
   * @brief Sets the display rotation used for coordinate mapping.
   *
   * @param rotation rotation value 0–3 (0=0°, 1=90°, 2=180°, 3=270°).
   */
  void setRotation(uint8_t rotation) { m_rotation = rotation; }

private:
  const uint8_t hello_packet[4] = {0x55, 0x55, 0x55, 0x55};

  /**
   * @brief Writes a register command to the touch controller over I2C.
   *
   * @param buff bytes to transmit.
   * @param size number of bytes to transmit.
   *
   * @return 0 on success, 1 on I2C error.
   */
  uint8_t tsWriteRegs(const uint8_t *buff, uint8_t size);

  /**
   * @brief Reads bytes from the touch controller over I2C.
   *
   * On error, zeroes the output buffer and logs the failure.
   *
   * @param buff output buffer.
   * @param size number of bytes to read.
   */
  void tsReadRegs(uint8_t *buff, uint8_t size);

  /**
   * @brief Issues a hardware reset pulse via the IO expander reset pin.
   */
  void tsHardwareReset();

  /**
   * @brief Issues a software reset command and verifies the hello packet
   * response.
   *
   * @return true if the controller responded with the expected hello packet.
   */
  bool tsSoftwareReset();

  /**
   * @brief Unpacks a raw 3-byte touch record into X and Y coordinates.
   *
   * @param d pointer to 3 raw bytes from the touch report.
   * @param x output X coordinate.
   * @param y output Y coordinate.
   */
  void tsGetXY(uint8_t *d, uint16_t *x, uint16_t *y);

  /**
   * @brief Reads the sensor's native X and Y resolution from the controller.
   *
   * @param xRes output X resolution.
   * @param yRes output Y resolution.
   */
  void tsGetResolution(uint16_t *xRes, uint16_t *yRes);

  /**
   * @brief Controls the touchscreen power supply and reset lines.
   *
   * @param enable true to power on, false to power off.
   */
  void power(bool enable);

  /**
   * @brief Deinitialises the touch controller and removes the interrupt
   * handler.
   */
  void end();

  i2c_master_dev_handle_t m_devHandle = nullptr;
  PCAL *m_expander = nullptr;

  uint16_t m_tsXResolution = 0;
  uint16_t m_tsYResolution = 0;
  uint8_t m_rotation = 0;

  uint8_t touchN = 0;
  uint16_t touchX[2] = {0, 0};
  uint16_t touchY[2] = {0, 0};
  uint32_t touchT = 0;
  bool m_tsInitDone = false;
};