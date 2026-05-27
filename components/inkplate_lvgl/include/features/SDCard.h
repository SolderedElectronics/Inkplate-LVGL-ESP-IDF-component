/**
 * @file SDCard.h
 * @author Fran Fodor for Soldered
 * @brief Helper for SD card communication.
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
#include "esp_err.h"
#include "sdmmc_cmd.h"

// SPI pin numbers (GPIO)
#if defined(CONFIG_INKPLATE_BOARD_INKPLATE13)
#define SD_MISO GPIO_NUM_13
#define SD_MOSI GPIO_NUM_11
#define SD_SCK GPIO_NUM_12
#define SD_CS GPIO_NUM_10
#else
#define SD_MISO GPIO_NUM_12
#define SD_MOSI GPIO_NUM_13
#define SD_SCK GPIO_NUM_14
#define SD_CS GPIO_NUM_15
#endif

#define SD_MOUNT_POINT "/sdcard"

/**
 * @brief Class for SD card communication.
 *
 */
class SDCard {
public:
  /**
   * @brief Construct a new SDCard object
   *
   * @param expander Reference to the PCAL expander that has SD_PMOS_PIN.
   * @param pin SD card pin.
   *
   * @note Store a reference to the IO expander used to control the SD power
   * switch.
   */
  SDCard(PCAL &expander, IOPin_t pin);

  /**
   * @brief Power on the SD card and mount the FAT filesystem via VFS.
   *
   * @return esp_err_t error code.
   */
  esp_err_t sdCardInit();

  /**
   * @brief Unmount the filesystem, free the SPI bus, and cut power to the card.
   *        Sets all SPI lines and the power switch pin as inputs to minimise
   * current draw.
   *
   * @return esp_err_t error code.
   */
  esp_err_t sdCardSleep();

  /**
   * @brief Get the mount point string for constructing file paths.
   *
   * @return const char* Mount point, e.g. "/sdcard".
   */
  const char *getMountPoint() { return SD_MOUNT_POINT; };

private:
  PCAL &m_expander;
  sdmmc_card_t *m_card = nullptr;
  IOPin_t m_pin;
};
