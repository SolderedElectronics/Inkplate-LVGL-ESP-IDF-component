/**
 * @file BME680.h
 * @author Fran Fodor for Soldered
 * @brief Wrapper for BME680 driver.
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
#include "libs/BME680Driver.h"

class BME680 : public BME680Driver {
public:
  bool begin(i2c_master_bus_handle_t bus_handle);

  float readTemperature();
  float readPressure();
  float readHumidity();
  float readAltitude();
  float readGasResistance();
  void readSensorData(float &temp, float &humidity, float &pressure,
                      float &gas);

  float calculateAltitude(float pressure);
};