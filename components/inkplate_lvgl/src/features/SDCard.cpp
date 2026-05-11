/**
 * @file SDCard.cpp
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

#include "driver/gpio.h"
#include "driver/sdspi_host.h"
#include "esp_rom_sys.h"
#include "esp_vfs_fat.h"

#include "SDCard.h"

/* -------------------------------------------------------------------------- */
/*                              Public functions                              */
/* -------------------------------------------------------------------------- */

SDCard::SDCard(PCAL &expander, IOPin_t pin)
    : m_expander(expander), m_pin(pin) {}

esp_err_t SDCard::sdCardInit() {
  m_expander.setDirection(m_pin, IO_MODE_OUTPUT, true);
  m_expander.setLevel(m_pin, 0, true);
  esp_rom_delay_us(50000);

  spi_bus_config_t busCfg = {};
  busCfg.mosi_io_num = SD_MOSI;
  busCfg.miso_io_num = SD_MISO;
  busCfg.sclk_io_num = SD_SCK;
  busCfg.quadwp_io_num = -1;
  busCfg.quadhd_io_num = -1;
  busCfg.max_transfer_sz = 4096;
  esp_err_t ret = spi_bus_initialize(SPI2_HOST, &busCfg, SDSPI_DEFAULT_DMA);
  if (ret != ESP_OK)
    return ret;

  sdmmc_host_t host = SDSPI_HOST_DEFAULT();
  host.slot = SPI2_HOST;

  sdspi_device_config_t slotCfg = SDSPI_DEVICE_CONFIG_DEFAULT();
  slotCfg.gpio_cs = SD_CS;
  slotCfg.host_id = SPI2_HOST;

  esp_vfs_fat_sdmmc_mount_config_t mountCfg = {};
  mountCfg.format_if_mount_failed = false;
  mountCfg.max_files = 5;
  mountCfg.allocation_unit_size = 16 * 1024;

  ret = esp_vfs_fat_sdspi_mount(SD_MOUNT_POINT, &host, &slotCfg, &mountCfg,
                                &m_card);
  if (ret != ESP_OK)
    return ret;

  sdmmc_card_print_info(stdout, m_card);
  return ret;
}

esp_err_t SDCard::sdCardSleep() {
  esp_err_t ret = ESP_OK;
  if (m_card) {
    ret = esp_vfs_fat_sdcard_unmount(SD_MOUNT_POINT, m_card);
    m_card = nullptr;
  }
  spi_bus_free(SPI2_HOST);

  gpio_set_direction(SD_MISO, GPIO_MODE_INPUT);
  gpio_set_direction(SD_MOSI, GPIO_MODE_INPUT);
  gpio_set_direction(SD_SCK, GPIO_MODE_INPUT);
  gpio_set_direction(SD_CS, GPIO_MODE_INPUT);
  m_expander.setDirection(m_pin, IO_MODE_INPUT, true);

  return ret;
}