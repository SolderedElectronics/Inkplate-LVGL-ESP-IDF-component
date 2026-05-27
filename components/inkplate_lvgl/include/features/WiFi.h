/**
 * @file WiFi.h
 * @author Fran Fodor for Soldered
 * @brief Helper for network communication.
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
#include "esp_event.h"
#include "stdint.h"

/**
 * @brief Class for network communication.
 *
 */
class WiFi {
public:
  /**
   * @brief Construct a new WiFi object.
   *
   */
  WiFi() = default;

  /**
   * @brief Initialize WiFi in STA mode with credentials from menuconfig.
   *
   * @return esp_err_t error code.
   */
  esp_err_t begin();

  /**
   * @brief Block until an IP is acquired or the timeout elapses.
   *
   * @param timeoutMs Maximum time to wait in milliseconds.
   * @return bool true if connected, false if timed out.
   */
  bool waitForConnect(uint32_t timeoutMs = 10000);

  /**
   * @brief Download a file via HTTP.
   *
   * @param url URL to download.
   * @param len In: assumed length if server doesn't provide Content-Length.
   *            Out: actual number of bytes downloaded.
   * @return uint8_t* pointer to allocated buffer, or NULL on failure.
   *         Caller is responsible for freeing the buffer.
   */
  uint8_t *downloadFile(const char *url, int32_t *len);

  /**
   * @brief Download a file via HTTPS.
   *
   * @param url URL to download.
   * @param len In: assumed length if server doesn't provide Content-Length.
   *            Out: actual number of bytes downloaded.
   * @return uint8_t* pointer to allocated buffer, or NULL on failure.
   *         Caller is responsible for freeing the buffer.
   */
  uint8_t *downloadFileHTTPS(const char *url, int32_t *len);

  /**
   * @brief Set current time in CET timezone using SNTP.
   *
   */
  void setCurrentTime();

  /**
   * @brief Set certificate.
   *
   * @param cert certificate.
   */
  void setCertificate(const char *cert) { m_certificate = cert; }

  /**
   * @brief Check if there is WiFi connection.
   *
   * @return bool true if there is connection.
   */
  bool isConnected() { return m_connected; }

  /**
   * @brief Check if time is set.
   *
   * @return bool true if time is set.
   */
  bool isTimeSet() { return m_timeSet; }

private:
  /**
   * @brief Handles IP events.
   *
   * @param arg unused.
   * @param event_base event base, expected to be IP_EVENT.
   * @param event_id IP_EVENT_STA_GOT_IP or IP_EVENT_STA_LOST_IP.
   * @param event_data unused.
   *
   * @note Sets the connection flag on IP acquired or lost.
   */
  static void wifiEventHandler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data);

  /**
   * @brief Handles WiFi events.
   *
   * @param arg unused.
   * @param event_base event base, expected to be WIFI_EVENT.
   * @param event_id WIFI_EVENT_STA_START or WIFI_EVENT_STA_DISCONNECTED.
   * @param event_data unused.
   *
   * @note Initiates connection on station start, and retries automatically on
   * disconnect.
   */
  static void ipEventHandler(void *arg, esp_event_base_t event_base,
                             int32_t event_id, void *event_data);

  static bool m_connected;
  bool m_timeSet = false;
  const char *m_certificate = nullptr;
};
