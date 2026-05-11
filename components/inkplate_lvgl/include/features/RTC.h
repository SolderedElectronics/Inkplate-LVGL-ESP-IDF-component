/**
 * @file RTC.h
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

#pragma once

#include "driver/i2c_master.h"
#include "time.h"

#define RTC_I2C_ADDR 0x51

// control registers
#define RTC_CTRL_1 0x00
#define RTC_CTRL_2 0x01
#define RTC_OFFSET 0x02
#define RTC_RAM 0x03

#define RTC_12_24 0x02

#define RTC_CTRL_2_DEFAULT 0x00
#define RTC_CTRL_1_DEFAULT 0x58

#define RTC_SET 0xAA
#define RTC_NOT_SET 0x00

// time and date registers
#define RTC_SECOND_ADDR 0x04
#define RTC_MINUTE_ADDR 0x05
#define RTC_HOUR_ADDR 0x06
#define RTC_DAY_ADDR 0x07
#define RTC_WDAY_ADDR 0x08
#define RTC_MONTH_ADDR 0x09
#define RTC_YEAR_ADDR 0x0A

// alarm registers
#define RTC_SECOND_ALARM 0x0B
#define RTC_MINUTE_ALARM 0x0C
#define RTC_HOUR_ALARM 0x0D
#define RTC_DAY_ALARM 0x0E
#define RTC_WDAY_ALARM 0x0F

// alarm control
#define RTC_ALARM_AIE 0x80
#define RTC_ALARM_AF 0x40

// timer registers
#define RTC_TIMER_VALUE 0x10
#define RTC_TIMER_MODE 0x11

// timer control
#define RTC_TIMER_TI_TP 0x01
#define RTC_TIMER_TIE 0x02
#define RTC_TIMER_TE 0x04
#define RTC_TIMER_TF 0x08

/**
 * @brief RTC clock source.
 *
 */
typedef enum {
  RTC_TIMER_CLOCK_4096HZ = 0,
  RTC_TIMER_CLOCK_64HZ = 1,
  RTC_TIMER_CLOCK_1HZ = 2,
  RTC_TIMER_CLOCK_1PER60HZ = 3,
} rtcCountdownSrcClock_t;

/**
 * @brief Hour format (12h or 24h).
 *
 */
typedef enum {
  RTC_FORMAT_24H = 0,
  RTC_FORMAT_12H = 1,
} rtcHourFormat_t;

/**
 * @brief Class for PCF85063A RTC.
 *
 */
class RTC {
public:
  /**
   * @brief Construct a new RTC object.
   *
   */
  RTC() = default;

  /**
   * @brief Add the RTC to the shared I2C bus and initialise it.
   *
   * @param bus_handle handle to the shared I2C master bus.
   * @return esp_err_t i2c error code.
   */
  esp_err_t begin(i2c_master_bus_handle_t bus_handle);

  /**
   * @brief Sets the time.
   *
   * @param time struct containing time information to set.
   * @return esp_err_t i2c error code.
   *
   * @note PCF85063A datasheet pg. 23; read or write access should be performed
   * in one go. If the full struct is not provided the values will be
   * undefinined (values of struct members that were not provided).
   */
  esp_err_t setTime(tm time);

  /**
   * @brief Get the time that was set.
   *
   * @param time pointer to struct to which time information will be written.
   * @return esp_err_t i2c error code.
   *
   * @note PCF85063A datasheet pg. 26; read or write access should be performed
   * in one go.
   */
  esp_err_t getTime(tm *time);

  /**
   * @brief Sets the time using epoch.
   *
   * @param epoch time in epoch.
   * @return esp_err_t i2c error code.
   *
   * @note PCF85063A datasheet pg. 23; read or write access should be performed
   * in one go. If the full struct is not provided the values will be
   * undefinined (values of struct members that were not provided).
   */
  esp_err_t setTime(time_t epoch);

  /**
   * @brief Get the time set in epoch.
   *
   * @param epoch pointer to epoch to which time information will be written.
   * @return esp_err_t i2c error code.
   *
   * @note PCF85063A datasheet pg. 26; read or write access should be performed
   * in one go.
   */
  esp_err_t getTime(time_t *epoch);

  /**
   * @brief Changes the RTC time format.
   *
   * @param format hour format to set.
   * @return esp_err_t i2c error code.
   */
  esp_err_t changeTimeFormat(rtcHourFormat_t format);

  /**
   * @brief Sets the alarm in seconds.
   *
   * @param second at what second to trigger the alarm.
   * @return esp_err_t i2c error code.
   */
  esp_err_t setAlarm(uint8_t second);

  /**
   * @brief Sets the alarm in seconds and minutes.
   *
   * @param second at what second to trigger the alarm.
   * @param minute at what minute to trigger the alarm.
   * @return esp_err_t i2c error code.
   */
  esp_err_t setAlarm(uint8_t second, uint8_t minute);

  /**
   * @brief Sets the alarm in seconds and minutes.
   *
   * @param second at what second to trigger the alarm.
   * @param minute at what minute to trigger the alarm.
   * @param hour at what hour to trigger the alarm.
   * @return esp_err_t i2c error code.
   */
  esp_err_t setAlarm(uint8_t second, uint8_t minute, uint8_t hour);

  /**
   * @brief Sets the alarm in seconds and minutes.
   *
   * @param second at what second to trigger the alarm.
   * @param minute at what minute to trigger the alarm.
   * @param hour at what hour to trigger the alarm.
   * @param day at what day to trigger the alarm.
   * @return esp_err_t i2c error code.
   */
  esp_err_t setAlarm(uint8_t second, uint8_t minute, uint8_t hour, uint8_t day);

  /**
   * @brief Sets the alarm in seconds and minutes.
   *
   * @param second at what second to trigger the alarm.
   * @param minute at what minute to trigger the alarm.
   * @param hour at what hour to trigger the alarm.
   * @param day at what day to trigger the alarm.
   * @param weekday at what weekday to trigger the alarm.
   * @return esp_err_t i2c error code.
   */
  esp_err_t setAlarm(uint8_t second, uint8_t minute, uint8_t hour, uint8_t day,
                     uint8_t weekday);

  /**
   * @brief Sets the alarm epoch.
   *
   * @param epoch when to trigger the alarm in epoch.
   * @return esp_err_t i2c error code.
   */
  esp_err_t setAlarmEpoch(time_t epoch);

  /**
   * @brief Gets the alarm values.
   *
   * @param time pointer to the struct in which the value will be stored.
   * @return esp_err_t i2c error code.
   *
   * @note Trying to display month or year will result in defined behavour as
   * alarm doesn't have month and year registers.
   */
  esp_err_t getAlarm(tm *time);

  /**
   * @brief Reads the value of alarm flag.
   *
   * @return esp_err_t i2c error code.
   */
  bool checkAlarmFlag();

  /**
   * @brief Cleares the alarm interrupt flag.
   *
   * @return esp_err_t i2c error code.
   *
   * @note Need to call this after every interrupt.
   */
  esp_err_t clearAlarmFlag();

  /**
   * @brief Sets the timer.
   *
   * @param clockSource timer clock frequency.
   * @param value timer value.
   * @param intEnable timer interrupt enable.
   * @param intPulse timer interrupt mode.
   * @return esp_err_t i2c error code.
   */
  esp_err_t setTimer(rtcCountdownSrcClock_t clockSource, uint8_t value,
                     bool intEnable, bool intPulse);

  /**
   * @brief Disables the timer.
   *
   * @return esp_err_t i2c error code.
   */
  esp_err_t disableTimer();

  /**
   * @brief Reads the value of timer flag.
   *
   * @return bool 0 if timer not triggered.
   */
  bool checkTimerFlag();

  /**
   * @brief Clear the timer interrupt flag.
   *
   * @return esp_err_t i2c error code.
   *
   * @note Need to call this after every interrupt.
   */
  esp_err_t clearTimerFlag();

  /**
   * @brief Reset RTC.
   *
   * @return esp_err_t i2c error code.
   */
  esp_err_t reset();

  /**
   * @brief Set internal capacitor value.
   *
   * @param value 0 or 1 which represents 7pF or 12.5pF.
   * @return esp_err_t i2c error code.
   */
  esp_err_t setInternalCapacitor(bool value);

  /**
   * @brief Offset used to correct frequency of the crystal used for RTC.
   *
   * @param mode 0 - offset made once every two hours, 1 - offset made every
   * four minutes.
   * @param offsetValue coded in two's complement
   * @return esp_err_t i2c error code.
   */
  esp_err_t setClockOffset(bool mode, int8_t offsetValue);

  /**
   * @brief Check if RTC is set.
   *
   * @return bool true if set.
   */
  bool isSet();

  /**
   * @brief Updates RTC and reads value.
   *
   * @return uint8_t second.
   */
  uint8_t getSecond();

  /**
   * @brief Updates RTC and reads value.
   *
   * @return uint8_t minute.
   */
  uint8_t getMinute();

  /**
   * @brief Updates RTC and reads value.
   *
   * @return uint8_t hour.
   */
  uint8_t getHour();

  /**
   * @brief Updates RTC and reads value.
   *
   * @return uint8_t day.
   */
  uint8_t getDay();

  /**
   * @brief Updates RTC and reads value.
   *
   * @return uint8_t weekday.
   */
  uint8_t getWeekday();

  /**
   * @brief Updates RTC and reads value.
   *
   * @return uint8_t month.
   */
  uint8_t getMonth();

  /**
   * @brief Updates RTC and reads value.
   *
   * @return uint8_t year.
   */
  uint16_t getYear();

  /**
   * @brief Reads alarm value.
   *
   * @return uint8_t alarm second.
   */
  uint8_t getAlarmSecond();

  /**
   * @brief Reads alarm value.
   *
   * @return uint8_t alarm minute.
   */
  uint8_t getAlarmMinute();

  /**
   * @brief Reads alarm value.
   *
   * @return uint8_t alarm hour.
   */
  uint8_t getAlarmHour();

  /**
   * @brief Reads alarm value.
   *
   * @return uint8_t alarm day.
   */
  uint8_t getAlarmDay();

  /**
   * @brief Reads alarm value.
   *
   * @return uint8_t alarm weekday.
   */
  uint8_t getAlarmWeekday();

private:
  /**
   * @brief Internal function to update values in the class.
   *
   * @return esp_err_t i2c error code.
   *
   * @note PCF85063A datasheet pg. 26; read or write access should be performed
   * in one go.
   */
  esp_err_t updateTime();

  /**
   * @brief Internal function to update values in the class.
   *
   * @return esp_err_t i2c error code.
   */
  esp_err_t updateAlarm();

  /**
   * @brief Internal function to enable alarm.
   *
   * @note PCF85063A datasheet pg. 11.
   */
  void enableAlarm();

  /**
   * @brief Handles hour conversion for different time formats.
   *
   * @param hour hour to convert.
   * @return uint8_t hour in Bcd.
   */
  uint8_t encodeHour(uint8_t hour);

  /**
   * @brief Converts decimal to BCD.
   *
   * @param value number which needs to be converted from decimal to Bcd value.
   * @return uint8_t value in Bcd.
   */
  uint8_t decToBcd(uint8_t value);
  /**
   * @brief Converts BCD to decimal.
   *
   * @param value number which needs to be converted from Bcd to decimal value.
   * @return uint8_t value in Dec.
   */
  uint8_t bcdToDec(uint8_t value);

  tm m_time;
  tm m_alarmTime;
  rtcHourFormat_t m_hourFormat;

  i2c_master_dev_handle_t m_devHandle;
};