/**
 * @file WiFi.cpp
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

#include "esp_heap_caps.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_sntp.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "string.h"
#include "time.h"

#include "WiFi.h"

static const char *TAG = "ESP_WIFI";

bool WiFi::m_connected = false;

/* -------------------------------------------------------------------------- */
/*                              Public functions                              */
/* -------------------------------------------------------------------------- */

esp_err_t WiFi::begin() {
  esp_err_t ret;

  ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  if (ret != ESP_OK)
    return ret;

  ret = esp_netif_init();
  if (ret != ESP_OK)
    return ret;

  ret = esp_event_loop_create_default();
  if (ret != ESP_OK)
    return ret;

  esp_netif_create_default_wifi_sta();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ret = esp_wifi_init(&cfg);
  if (ret != ESP_OK)
    return ret;

  ret = esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                            &wifiEventHandler, NULL, NULL);
  if (ret != ESP_OK)
    return ret;

  ret = esp_event_handler_instance_register(IP_EVENT, ESP_EVENT_ANY_ID,
                                            &ipEventHandler, NULL, NULL);
  if (ret != ESP_OK)
    return ret;

  if (strlen(CONFIG_WIFI_SSID) == 0) {
    ESP_LOGE(TAG, "WiFi SSID not set. Run menuconfig.");
    return ESP_ERR_INVALID_ARG;
  }

  wifi_config_t wifi_config = {};
  memcpy(wifi_config.sta.ssid, CONFIG_WIFI_SSID, sizeof(CONFIG_WIFI_SSID));
  memcpy(wifi_config.sta.password, CONFIG_WIFI_PASSWORD,
         sizeof(CONFIG_WIFI_PASSWORD));

  ret = esp_wifi_set_mode(WIFI_MODE_STA);
  if (ret != ESP_OK)
    return ret;

  ret = esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
  if (ret != ESP_OK)
    return ret;

  ret = esp_wifi_start();
  if (ret != ESP_OK)
    return ret;

  ESP_LOGI(TAG, "WiFi initialization finished!");
  return ESP_OK;
}

bool WiFi::waitForConnect(uint32_t timeoutMs) {
  uint32_t elapsed = 0;
  while (!m_connected && elapsed < timeoutMs) {
    vTaskDelay(pdMS_TO_TICKS(100));
    elapsed += 100;
  }
  return m_connected;
}

uint8_t *WiFi::downloadFile(const char *url, int32_t *len) {
  if (!waitForConnect()) {
    ESP_LOGE(TAG, "No WiFi connection");
    return NULL;
  }

  ESP_LOGI(TAG, "Downloading file via HTTP");

  esp_http_client_config_t config = {};
  config.url = url;
  config.timeout_ms = 10000;

  esp_http_client_handle_t client = esp_http_client_init(&config);
  if (!client)
    return NULL;

  esp_err_t err = esp_http_client_open(client, 0);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "HTTP open failed: %s", esp_err_to_name(err));
    esp_http_client_cleanup(client);
    return NULL;
  }

  int32_t contentLen = (int32_t)esp_http_client_fetch_headers(client);
  if (contentLen <= 0)
    contentLen = *len;
  else
    *len = contentLen;

  uint8_t *buffer = (uint8_t *)heap_caps_malloc(
      contentLen, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  if (!buffer) {
    ESP_LOGE(TAG, "Failed to allocate %ld bytes", contentLen);
    esp_http_client_cleanup(client);
    return NULL;
  }

  int32_t totalRead = 0;
  uint8_t chunk[512];
  int read;
  while (totalRead < contentLen) {
    read = esp_http_client_read(client, (char *)chunk, sizeof(chunk));
    if (read <= 0)
      break;
    memcpy(buffer + totalRead, chunk, read);
    totalRead += read;
  }

  esp_http_client_cleanup(client);
  *len = totalRead;

  ESP_LOGI(TAG, "File downloaded");

  return buffer;
}

uint8_t *WiFi::downloadFileHTTPS(const char *url, int32_t *len) {
  if (!waitForConnect()) {
    ESP_LOGE(TAG, "No WiFi connection");
    return NULL;
  }

  ESP_LOGI(TAG, "Downloading file via HTTPS");

  esp_http_client_config_t config = {};
  config.url = url;
  config.timeout_ms = 10000;
  config.transport_type = HTTP_TRANSPORT_OVER_SSL;

  if (m_certificate)
    config.cert_pem = m_certificate;

  esp_http_client_handle_t client = esp_http_client_init(&config);
  if (!client)
    return NULL;

  esp_err_t err = esp_http_client_open(client, 0);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "HTTPS open failed: %s", esp_err_to_name(err));
    esp_http_client_cleanup(client);
    return NULL;
  }

  int32_t contentLen = (int32_t)esp_http_client_fetch_headers(client);
  if (contentLen <= 0)
    contentLen = *len;
  else
    *len = contentLen;

  uint8_t *buffer = (uint8_t *)heap_caps_malloc(
      contentLen, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  if (!buffer) {
    ESP_LOGE(TAG, "Failed to allocate %ld bytes", contentLen);
    esp_http_client_cleanup(client);
    return NULL;
  }

  int32_t totalRead = 0;
  uint8_t chunk[512];
  int read;

  while (totalRead < contentLen) {
    read = esp_http_client_read(client, (char *)chunk, sizeof(chunk));
    if (read <= 0)
      break;
    memcpy(buffer + totalRead, chunk, read);
    totalRead += read;
  }

  esp_http_client_cleanup(client);
  *len = totalRead;

  ESP_LOGI(TAG, "File downloaded");

  return buffer;
}

void WiFi::setCurrentTime() {
  esp_sntp_stop();

  esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
  esp_sntp_setservername(0, "pool.ntp.org");
  esp_sntp_init();

  while (esp_sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET) {
    ESP_LOGI(TAG, "Waiting for NTP sync...");
    vTaskDelay(pdMS_TO_TICKS(2000));
  }

  setenv("TZ", "CST-2", 1);
  tzset();

  m_timeSet = true;
}

/* -------------------------------------------------------------------------- */
/*                              Private functions                             */
/* -------------------------------------------------------------------------- */

void WiFi::ipEventHandler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data) {
  if (event_id == IP_EVENT_STA_GOT_IP) {
    ESP_LOGI(TAG, "IP acquired");
    m_connected = true;
  }
  if (event_id == IP_EVENT_STA_LOST_IP) {
    ESP_LOGI(TAG, "IP lost");
    m_connected = false;
  }
}

void WiFi::wifiEventHandler(void *arg, esp_event_base_t event_base,
                            int32_t event_id, void *event_data) {
  if (event_id == WIFI_EVENT_STA_START) {
    esp_wifi_connect();
  } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
    ESP_LOGI(TAG, "Disconnected, retrying...");
    m_connected = false;
    esp_wifi_connect();
  }
}
