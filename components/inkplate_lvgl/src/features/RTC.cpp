/**
 * @file RTC.cpp
 * @author Fran Fodor for Soldered
 * @brief Driver for PCF85063A RTC.
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

#include "esp_log.h"
#include "time.h"

#include "RTC.h"

static const char *TAG = "ESP_RTC";

/* -------------------------------------------------------------------------- */
/*                              Public functions                              */
/* -------------------------------------------------------------------------- */

esp_err_t RTC::begin(i2c_master_bus_handle_t bus_handle) {
  i2c_device_config_t dev_cfg = {};
  dev_cfg.dev_addr_length = I2C_ADDR_BIT_LEN_7;
  dev_cfg.device_address = RTC_I2C_ADDR;
  dev_cfg.scl_speed_hz = 400000;

  esp_err_t ret = i2c_master_bus_add_device(bus_handle, &dev_cfg, &m_devHandle);
  if (ret != ESP_OK)
    return ret;

  m_hourFormat = RTC_FORMAT_24H;

  // Only clear the RAM flag if it hasn't been set yet,
  // so it survives deep sleep wake-ups
  uint8_t reg = RTC_RAM;
  uint8_t ramByte = 0;
  ret = i2c_master_transmit_receive(m_devHandle, &reg, 1, &ramByte, 1, -1);
  if (ret != ESP_OK)
    return ret;

  if (ramByte != RTC_SET) {
    uint8_t data[2] = {RTC_RAM, RTC_NOT_SET};
    ret = i2c_master_transmit(m_devHandle, data, sizeof(data), -1);
  }

  ESP_LOGI(TAG, "I2C initilization finished!");
  return ret;
}

esp_err_t RTC::setTime(tm time) {
  esp_err_t ret = ESP_OK;

  uint8_t data[8] = {RTC_SECOND_ADDR,           decToBcd(time.tm_sec),
                     decToBcd(time.tm_min),     encodeHour(time.tm_hour),
                     decToBcd(time.tm_mday),    decToBcd(time.tm_wday),
                     decToBcd(time.tm_mon - 1), decToBcd(time.tm_year % 100)};

  ret = i2c_master_transmit(m_devHandle, data, sizeof(data), -1);
  if (ret != ESP_OK)
    return ret;

  uint8_t data_[2] = {RTC_RAM, RTC_SET};
  ret = i2c_master_transmit(m_devHandle, data_, sizeof(data_), -1);

  return ret;
}

esp_err_t RTC::getTime(tm *time) {
  esp_err_t ret = updateTime();
  if (ret != ESP_OK)
    return ret;

  memcpy(time, &m_time, sizeof(tm));

  return ret;
}

esp_err_t RTC::setTime(time_t epoch) {
  tm time;
  localtime_r(&epoch, &time);
  time.tm_year += 1900;

  return setTime(time);
}

esp_err_t RTC::getTime(time_t *epoch) {
  esp_err_t ret = updateTime();
  if (ret != ESP_OK)
    return ret;

  tm t = m_time;
  t.tm_year -= 1900; // 2023 - 1900 = 123, correct for mktime
  t.tm_mon -= 1;     // driver stores 1-based, mktime needs 0-based
  *epoch = mktime(&t);

  return ret;
}

esp_err_t RTC::changeTimeFormat(rtcHourFormat_t format) {
  esp_err_t ret = ESP_OK;

  uint8_t reg = RTC_CTRL_1;
  uint8_t ctrl1 = 0;

  ret = i2c_master_transmit_receive(m_devHandle, &reg, 1, &ctrl1, 1, -1);
  if (ret != ESP_OK)
    return ret;

  if (format == RTC_FORMAT_12H)
    ctrl1 |= RTC_12_24;
  else
    ctrl1 &= ~RTC_12_24;

  uint8_t data[2] = {RTC_CTRL_1, ctrl1};

  ret = i2c_master_transmit(m_devHandle, data, sizeof(data), -1);
  if (ret != ESP_OK)
    return ret;

  m_hourFormat = format;

  return ret;
}

esp_err_t RTC::setAlarm(uint8_t second) {
  esp_err_t ret = ESP_OK;
  enableAlarm();

  uint8_t data[2] = {RTC_SECOND_ALARM,
                     (uint8_t)(decToBcd(second) & ~RTC_ALARM_AIE)};

  ret = i2c_master_transmit(m_devHandle, data, sizeof(data), -1);
  return ret;
}

esp_err_t RTC::setAlarm(uint8_t second, uint8_t minute) {
  esp_err_t ret = ESP_OK;
  enableAlarm();

  uint8_t data[3] = {RTC_SECOND_ALARM,
                     (uint8_t)(decToBcd(second) & ~RTC_ALARM_AIE),
                     (uint8_t)(decToBcd(minute) & ~RTC_ALARM_AIE)};

  ret = i2c_master_transmit(m_devHandle, data, sizeof(data), -1);
  return ret;
}

esp_err_t RTC::setAlarm(uint8_t second, uint8_t minute, uint8_t hour) {
  esp_err_t ret = ESP_OK;
  enableAlarm();

  uint8_t data[4] = {RTC_SECOND_ALARM,
                     (uint8_t)(decToBcd(second) & ~RTC_ALARM_AIE),
                     (uint8_t)(decToBcd(minute) & ~RTC_ALARM_AIE),
                     (uint8_t)(encodeHour(hour) & ~RTC_ALARM_AIE)};

  ret = i2c_master_transmit(m_devHandle, data, sizeof(data), -1);
  return ret;
}

esp_err_t RTC::setAlarm(uint8_t second, uint8_t minute, uint8_t hour,
                        uint8_t mday) {
  esp_err_t ret = ESP_OK;
  enableAlarm();

  uint8_t data[5] = {RTC_SECOND_ALARM,
                     (uint8_t)(decToBcd(second) & ~RTC_ALARM_AIE),
                     (uint8_t)(decToBcd(minute) & ~RTC_ALARM_AIE),
                     (uint8_t)(encodeHour(hour) & ~RTC_ALARM_AIE),
                     (uint8_t)(decToBcd(mday) & ~RTC_ALARM_AIE)};

  ret = i2c_master_transmit(m_devHandle, data, sizeof(data), -1);
  return ret;
}

esp_err_t RTC::setAlarm(uint8_t second, uint8_t minute, uint8_t hour,
                        uint8_t mday, uint8_t wday) {
  esp_err_t ret = ESP_OK;
  enableAlarm();

  uint8_t data[6] = {RTC_SECOND_ALARM,
                     (uint8_t)(decToBcd(second) & ~RTC_ALARM_AIE),
                     (uint8_t)(decToBcd(minute) & ~RTC_ALARM_AIE),
                     (uint8_t)(encodeHour(hour) & ~RTC_ALARM_AIE),
                     (uint8_t)(decToBcd(mday) & ~RTC_ALARM_AIE),
                     (uint8_t)(decToBcd(wday) & ~RTC_ALARM_AIE)};

  ret = i2c_master_transmit(m_devHandle, data, sizeof(data), -1);
  return ret;
}

esp_err_t RTC::setAlarmEpoch(time_t epoch) {
  tm time;
  gmtime_r(&epoch, &time);

  return setAlarm(time.tm_sec, time.tm_min, time.tm_hour, time.tm_mday,
                  time.tm_wday);
}

esp_err_t RTC::getAlarm(tm *time) {
  esp_err_t ret = ESP_OK;

  uint8_t reg = RTC_SECOND_ALARM;
  uint8_t data[5];

  ret =
      i2c_master_transmit_receive(m_devHandle, &reg, 1, data, sizeof(data), -1);
  if (ret != ESP_OK)
    return ret;

  m_alarmTime.tm_sec = bcdToDec(data[0]);
  m_alarmTime.tm_min = bcdToDec(data[1]);
  m_alarmTime.tm_hour = bcdToDec(data[2]);
  m_alarmTime.tm_mday = bcdToDec(data[3]);
  m_alarmTime.tm_wday = bcdToDec(data[4]);

  memcpy(time, &m_alarmTime, sizeof(tm));

  return ret;
}

bool RTC::checkAlarmFlag() {
  uint8_t reg = RTC_CTRL_2;
  uint8_t ctrl2 = 0;

  ESP_ERROR_CHECK(
      i2c_master_transmit_receive(m_devHandle, &reg, 1, &ctrl2, 1, -1));

  return (ctrl2 & RTC_ALARM_AF) != 0;
}

esp_err_t RTC::clearAlarmFlag() {
  esp_err_t ret = ESP_OK;

  uint8_t reg = RTC_CTRL_2;
  uint8_t ctrl2 = 0;

  ret = i2c_master_transmit_receive(m_devHandle, &reg, 1, &ctrl2, 1, -1);
  if (ret != ESP_OK)
    return ret;

  // clear AF bit and write back
  uint8_t data[2] = {RTC_CTRL_2, (uint8_t)(ctrl2 & ~RTC_ALARM_AF)};
  ret = i2c_master_transmit(m_devHandle, data, sizeof(data), -1);

  return ret;
}

esp_err_t RTC::setTimer(rtcCountdownSrcClock_t sourceClock, uint8_t value,
                        bool intEnable, bool intPulse) {
  esp_err_t ret = ESP_OK;

  uint8_t data[2] = {RTC_CTRL_2, 0x00};

  ret = i2c_master_transmit(m_devHandle, data, sizeof(data), -1);
  if (ret != ESP_OK)
    return ret;

  uint8_t mode = RTC_TIMER_TE;
  if (intEnable)
    mode |= RTC_TIMER_TIE;
  if (intPulse)
    mode |= RTC_TIMER_TI_TP;
  mode |= sourceClock << 3;

  uint8_t data_[3] = {RTC_TIMER_VALUE, value, mode};

  ret = i2c_master_transmit(m_devHandle, data_, sizeof(data_), -1);

  return ret;
}

esp_err_t RTC::disableTimer() {
  esp_err_t ret = ESP_OK;

  uint8_t reg = RTC_TIMER_MODE;
  uint8_t mode = 0;

  ret = i2c_master_transmit_receive(m_devHandle, &reg, 1, &mode, 1, -1);
  if (ret != ESP_OK)
    return ret;

  uint8_t data[2] = {RTC_TIMER_MODE, (uint8_t)(mode & ~RTC_TIMER_TE)};

  ret = i2c_master_transmit(m_devHandle, data, sizeof(data), -1);

  return ret;
}

bool RTC::checkTimerFlag() {
  uint8_t reg = RTC_CTRL_2;
  uint8_t ctrl2 = 0;

  ESP_ERROR_CHECK(
      i2c_master_transmit_receive(m_devHandle, &reg, 1, &ctrl2, 1, -1));

  return (ctrl2 & RTC_TIMER_TF) != 0;
}

esp_err_t RTC::clearTimerFlag() {
  esp_err_t ret = ESP_OK;

  uint8_t reg = RTC_CTRL_2;
  uint8_t ctrl2 = 0;

  ret = i2c_master_transmit_receive(m_devHandle, &reg, 1, &ctrl2, 1, -1);
  if (ret != ESP_OK)
    return ret;

  // clear TF bit and write back
  uint8_t data[2] = {RTC_CTRL_2, (uint8_t)(ctrl2 & ~RTC_TIMER_TF)};
  ret = i2c_master_transmit(m_devHandle, data, sizeof(data), -1);

  return ret;
}

esp_err_t RTC::reset() {
  esp_err_t ret = ESP_OK;

  uint8_t data[2] = {RTC_CTRL_1, RTC_CTRL_1_DEFAULT};

  ret = i2c_master_transmit(m_devHandle, data, sizeof(data), -1);

  return ret;
}

esp_err_t RTC::setInternalCapacitor(bool value) {
  esp_err_t ret;

  uint8_t reg = RTC_CTRL_1;
  uint8_t ctrl1 = 0;

  ret = i2c_master_transmit_receive(m_devHandle, &reg, 1, &ctrl1, 1, -1);
  if (ret != ESP_OK)
    return ret;

  if (value)
    ctrl1 |= (1 << 0);
  else
    ctrl1 &= ~(1 << 0);

  uint8_t data[2] = {RTC_CTRL_1, ctrl1};

  ret = i2c_master_transmit(m_devHandle, data, sizeof(data), -1);

  return ret;
}

esp_err_t RTC::setClockOffset(bool mode, int8_t offsetValue) {
  if (offsetValue > 63 || offsetValue < -64)
    return ESP_ERR_INVALID_ARG;

  if (offsetValue < 0)
    offsetValue += 128;

  uint8_t regValue = (uint8_t)offsetValue;
  if (mode)
    regValue |= (1 << 7);
  else
    regValue &= ~(1 << 7);

  uint8_t data[2] = {RTC_OFFSET, regValue};

  return i2c_master_transmit(m_devHandle, data, sizeof(data), -1);
}

bool RTC::isSet() {
  uint8_t reg = RTC_RAM;
  uint8_t ramByte = 0;

  ESP_ERROR_CHECK(
      i2c_master_transmit_receive(m_devHandle, &reg, 1, &ramByte, 1, -1));

  return ramByte == RTC_SET;
}

uint8_t RTC::getSecond() {
  updateTime();
  return m_time.tm_sec;
}

uint8_t RTC::getMinute() {
  updateTime();
  return m_time.tm_min;
}

uint8_t RTC::getHour() {
  updateTime();
  return m_time.tm_hour;
}

uint8_t RTC::getDay() {
  updateTime();
  return m_time.tm_mday;
}

uint8_t RTC::getWeekday() {
  updateTime();
  return m_time.tm_wday;
}

uint8_t RTC::getMonth() {
  updateTime();
  return m_time.tm_mon;
}

uint16_t RTC::getYear() {
  updateTime();
  return m_time.tm_year;
}

uint8_t RTC::getAlarmSecond() {
  updateAlarm();
  return m_time.tm_sec;
}

uint8_t RTC::getAlarmMinute() {
  updateAlarm();
  return m_time.tm_min;
}

uint8_t RTC::getAlarmHour() {
  updateAlarm();
  return m_time.tm_hour;
}

uint8_t RTC::getAlarmDay() {
  updateAlarm();
  return m_time.tm_mday;
}

uint8_t RTC::getAlarmWeekday() {
  updateAlarm();
  return m_time.tm_wday;
}

/* -------------------------------------------------------------------------- */
/*                              Private functions                             */
/* -------------------------------------------------------------------------- */

esp_err_t RTC::updateTime() {
  esp_err_t ret = ESP_OK;

  uint8_t reg = RTC_SECOND_ADDR;
  uint8_t data[7] = {0};

  ret = i2c_master_transmit(m_devHandle, &reg, 1, -1);
  if (ret != ESP_OK)
    return ret;
  ret = i2c_master_receive(m_devHandle, data, sizeof(data), -1);
  if (ret != ESP_OK)
    return ret;

  // check datasheet to see unused bits in registers
  m_time.tm_sec = bcdToDec(data[0] & 0x7F);
  m_time.tm_min = bcdToDec(data[1] & 0x7F);
  // m_time.tm_hour = bcdToDec(data[2] & 0x3F);
  m_time.tm_mday = bcdToDec(data[3] & 0x3F);
  m_time.tm_wday = bcdToDec(data[4] & 0x07);
  m_time.tm_mon = bcdToDec(data[5] & 0x1F) + 1;
  m_time.tm_year = bcdToDec(data[6]) + 2000;

  if (m_hourFormat == RTC_FORMAT_12H) {
    bool pm = (data[2] & 0x20) != 0;
    uint8_t hour = bcdToDec(data[2] & 0x1F);

    if (pm && hour != 12)
      hour += 12;
    if (!pm && hour == 12)
      hour = 0;
    m_time.tm_hour = hour;
  } else {
    m_time.tm_hour = bcdToDec(data[2] & 0x3F);
  }

  return ret;
}

esp_err_t RTC::updateAlarm() {
  esp_err_t ret = ESP_OK;
  uint8_t reg = RTC_SECOND_ALARM;
  uint8_t data[5] = {0};
  ret =
      i2c_master_transmit_receive(m_devHandle, &reg, 1, data, sizeof(data), -1);
  if (ret != ESP_OK)
    return ret;

  m_alarmTime.tm_sec = bcdToDec(data[0] & 0x7F);
  m_alarmTime.tm_min = bcdToDec(data[1] & 0x7F);
  m_alarmTime.tm_mday = bcdToDec(data[3] & 0x3F);
  m_alarmTime.tm_wday = bcdToDec(data[4] & 0x07);

  if (m_hourFormat == RTC_FORMAT_12H) {
    bool pm = (data[2] & 0x20) != 0;
    uint8_t hour = bcdToDec(data[2] & 0x1F);

    if (pm && hour != 12)
      hour += 12;
    if (!pm && hour == 12)
      hour = 0;
    m_alarmTime.tm_hour = hour;
  } else {
    m_alarmTime.tm_hour = bcdToDec(data[2] & 0x3F);
  }

  return ret;
}

void RTC::enableAlarm() {
  uint8_t registerMask;
  registerMask = (RTC_CTRL_2_DEFAULT | RTC_ALARM_AIE) & ~RTC_ALARM_AF;

  uint8_t data[2] = {RTC_CTRL_2, registerMask};

  ESP_ERROR_CHECK(i2c_master_transmit(m_devHandle, data, 2, -1));
}

uint8_t RTC::encodeHour(uint8_t hour) {
  if (m_hourFormat == RTC_FORMAT_12H) {
    bool pm = hour >= 12;
    uint8_t hour12 = hour % 12;
    if (hour12 == 0)
      hour12 = 12;
    return decToBcd(hour12) | (pm ? 0x20 : 0x00);
  }

  return decToBcd(hour);
}

uint8_t RTC::decToBcd(uint8_t val) { return ((val / 10 * 16) + (val % 10)); }

uint8_t RTC::bcdToDec(uint8_t val) { return ((val / 16 * 10) + (val % 16)); }
