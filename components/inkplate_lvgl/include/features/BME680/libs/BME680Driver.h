// clang-format off
/*! @file Zanshin_BME680.h

@mainpage Arduino Library to access and control a Bosch BME680 Environmental Sensor

@section Zanshin_BME680_section Description

Class definition header for the Bosch BME680 temperature / humidity / pressure / air quality sensor.
The device is described at https://www.bosch-sensortec.com/bst/products/all_products/BME680 and the
datasheet is available from Bosch at
https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BME680-DS001-00.pdf
\n\n

The BME680 uses I2C for communications. This library supports I2C using the ESP-IDF i2c_master
driver. Depending upon the pin configuration a BME680 can have 2 distinct I2C addresses - either
0x76 or 0x77.\n\n

The most recent version of the library is available at https://github.com/Zanduino/BME680 and
extensive documentation of the library as well as descriptions of the various example programs are
described in the project's wiki pages located at https://github.com/Zanduino/BME680/wiki

@section license GNU General Public License v3.0

This program is free software: you can redistribute it and/or modify it under the terms of the GNU
General Public License as published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version. This program is distributed in the hope that it
will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details. You should
have received a copy of the GNU General Public License along with this program.  If not, see
<http://www.gnu.org/licenses/>.

@section author Author

Written by Arnd <Arnd@Zanduino.Com> at https://www.github.com/SV-Zanshin

*/
// clang-format on

#include "driver/i2c_master.h"
#include <stdint.h>
#include <string.h>

#ifndef BME680Driver_h
#define BME680Driver_h

#define CONCAT_BYTES(msb, lsb)                                                 \
  (((uint16_t)msb << 8) | (uint16_t)lsb) ///< combine msb & lsb bytes
#ifndef _BV
#define _BV(bit) (1 << (bit)) ///< This macro isn't pre-defined on all platforms
#endif

/***************************************************************************************************
** Declare publically visible constants used in the class                   **
***************************************************************************************************/
#ifndef I2C_MODES
#define I2C_MODES
const uint32_t I2C_STANDARD_MODE{100000};   ///< Default normal I2C 100KHz speed
const uint32_t I2C_FAST_MODE{400000};       ///< Fast mode
const uint32_t I2C_FAST_MODE_PLUS{1000000}; ///< Really fast mode
const uint32_t I2C_HIGH_SPEED_MODE{3400000}; ///< Turbo mode
#endif

/***************************************************************************************************
** Declare enumerated types used in the class                   **
***************************************************************************************************/
/*! @brief  Enumerate the sensor type */
enum sensorTypes {
  TemperatureSensor,
  HumiditySensor,
  PressureSensor,
  GasSensor,
  UnknownSensor
};
/*! @brief  Enumerate the Oversampling types */
enum oversamplingTypes {
  SensorOff,
  Oversample1,
  Oversample2,
  Oversample4,
  Oversample8,
  Oversample16,
  UnknownOversample
};
/*! @brief  Enumerate the iir filter types */
enum iirFilterTypes {
  IIROff,
  IIR2,
  IIR4,
  IIR8,
  IIR16,
  IIR32,
  IIR64,
  IIR128,
  UnknownIIR
};

class BME680Driver {
public:
  BME680Driver();
  ~BME680Driver();
  bool begin(i2c_master_bus_handle_t busHandle, uint8_t i2cAddress = 0);
  uint8_t setOversampling(const uint8_t sensor,
                          const uint8_t sampling = UINT8_MAX) const;
  bool setGas(uint16_t GasTemp, uint16_t GasMillis) const;
  uint8_t setIIRFilter(const uint8_t iirFilterSetting = UINT8_MAX) const;
  uint8_t getSensorData(int32_t &temp, int32_t &hum, int32_t &press,
                        int32_t &gas, const bool waitSwitch = true);
  uint8_t getI2CAddress() const;
  void reset();
  bool measuring() const;
  void triggerMeasurement() const;
  uint8_t readByte(const uint8_t addr) const;

  /*!
   @section Template functions
   getData / putData — all device I/O goes through these two functions.
   The address and a variable are passed; the functions determine the
   size of the variable and read or write exactly that many bytes.
  */
  template <typename T> uint8_t &getData(const uint8_t addr, T &value) const {
    /*!
      @brief     Template for reading from I2C using any data type
      @param[in] addr Register address
      @param[in] value Data Type "T" to read into
      @return    Number of bytes read
    */
    uint8_t *bytePtr = (uint8_t *)&value;
    static uint8_t structSize = sizeof(T);
    if (_i2cDevHandle) {
      i2c_master_transmit_receive(_i2cDevHandle, &addr, 1, bytePtr, sizeof(T),
                                  -1);
      structSize = sizeof(T);
    }
    return structSize;
  } // of method getData()

  template <typename T>
  uint8_t &putData(const uint8_t addr, const T &value) const {
    /*!
      @brief     Template for writing to I2C using any data type
      @param[in] addr Register address
      @param[in] value Data Type "T" to write
      @return    Number of bytes written
    */
    const uint8_t *bytePtr = (const uint8_t *)&value;
    static uint8_t structSize = sizeof(T);
    if (_i2cDevHandle) {
      uint8_t buf[sizeof(T) + 1];
      buf[0] = addr;
      memcpy(buf + 1, bytePtr, sizeof(T));
      i2c_master_transmit(_i2cDevHandle, buf, sizeof(T) + 1, -1);
    }
    return structSize;
  } // of method putData()

private:
  bool commonInitialization();
  uint8_t readSensors(const bool waitSwitch);
  void waitForReadings() const;
  void getCalibration();

  uint8_t _I2CAddress = 0; ///< I2C address of the device (0 = not found)
  uint32_t _I2CSpeed = 0;  ///< I2C bus speed in Hz

  i2c_master_dev_handle_t _i2cDevHandle =
      nullptr; ///< ESP-IDF I2C device handle

  uint8_t _H6 = 0, _P10 = 0, _res_heat_range = 0;
  int8_t _H3 = 0, _H4 = 0, _H5 = 0, _H7 = 0;
  int8_t _G1 = 0, _G3 = 0, _T3 = 0;
  int8_t _P3 = 0, _P6 = 0, _P7 = 0, _res_heat = 0, _rng_sw_err = 0;
  uint16_t _H1 = 0, _H2 = 0, _T1 = 0, _P1 = 0;
  int16_t _G2 = 0, _T2 = 0, _P2 = 0, _P4 = 0, _P5 = 0, _P8 = 0, _P9 = 0;
  int32_t _tfine = 0, _Temperature = 0, _Pressure = 0, _Humidity = 0, _Gas = 0;
}; // of BME680Driver definition

#endif
