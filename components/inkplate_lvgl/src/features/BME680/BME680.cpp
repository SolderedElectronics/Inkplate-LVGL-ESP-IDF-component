/**
 **************************************************
 *
 * @file        BME680-SOLDERED.cpp
 * @brief       Simplified BME680 wrapper for Soldered board (ESP-IDF)
 *
 *
 * @copyright GNU General Public License v3.0
 * @authors     Zvonimir Haramustek for Soldered.com
 ***************************************************/

#include "BME680.h"
#include <math.h>

/**
 * @brief  Register the BME680 on an existing I2C bus and configure it.
 *
 * @param bus_handle  Handle to an already-initialised I2C master bus.
 * @return true on success
 */
bool BME680::begin(i2c_master_bus_handle_t bus_handle) {
  bool returnValue = BME680Driver::begin(bus_handle, 0);

  if (returnValue) {
    BME680Driver::setOversampling(TemperatureSensor, Oversample16);
    BME680Driver::setOversampling(HumiditySensor, Oversample16);
    BME680Driver::setOversampling(PressureSensor, Oversample16);
    BME680Driver::setIIRFilter(IIR4);
    BME680Driver::setGas(320, 150);
  }

  return returnValue;
}

/**
 * @brief         BME680 temperature method
 * @returns float Temperature in degree C
 */
float BME680::readTemperature() {
  int32_t temp, humidity, pressure, gas;
  BME680Driver::getSensorData(temp, humidity, pressure, gas);
  return temp / 100.0f;
}

/**
 * @brief         BME680 Pressure method
 * @returns float Pressure in hPa
 */
float BME680::readPressure() {
  int32_t temp, humidity, pressure, gas;
  BME680Driver::getSensorData(temp, humidity, pressure, gas);
  return pressure / 100.0f;
}

/**
 * @brief         BME680 Humidity method
 * @returns float Humidity in %
 */
float BME680::readHumidity() {
  int32_t temp, humidity, pressure, gas;
  BME680Driver::getSensorData(temp, humidity, pressure, gas);
  return humidity / 1000.0f;
}

/**
 * @brief         BME680 Altitude method
 * @returns float Altitude in m
 */
float BME680::readAltitude() {
  int32_t temp, humidity, pressure, gas;
  BME680Driver::getSensorData(temp, humidity, pressure, gas);
  const float seaLevel = 1013.25f;
  return 44330.0f *
         (1.0f - powf(((float)pressure / 100.0f) / seaLevel, 0.1903f));
}

/**
 * @brief         BME680 Gas resistance method, default at 320 degrees for 150
 * ms
 * @returns float Gas resistance in mOhms
 */
float BME680::readGasResistance() {
  int32_t temp, humidity, pressure, gas;
  BME680Driver::getSensorData(temp, humidity, pressure, gas);
  return gas / 100.0f;
}

/**
 * @brief                    BME680 all sensor data method
 * @param float& temp        Temperature in degree C
 * @param float& humidity    Humidity in %
 * @param float& pressure    Pressure in hPa
 * @param float& gas         Gas resistance in mOhms
 */
void BME680::readSensorData(float &temp, float &humidity, float &pressure,
                            float &gas) {
  int32_t _temp, _humidity, _pressure, _gas;
  BME680Driver::getSensorData(_temp, _humidity, _pressure, _gas);

  temp = _temp / 100.0f;
  humidity = _humidity / 1000.0f;
  pressure = _pressure / 100.0f;
  gas = _gas / 100.0f;
}

/**
 * @brief                BME680 altitude calculation method
 * @param float pressure Pressure in hPa
 * @returns float        Altitude in m
 */
float BME680::calculateAltitude(float pressure) {
  const float seaLevel = 1013.25f;
  return 44330.0f * (1.0f - powf(pressure / seaLevel, 0.1903f));
}
