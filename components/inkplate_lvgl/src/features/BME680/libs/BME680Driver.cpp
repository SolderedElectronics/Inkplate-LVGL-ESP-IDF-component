/*! @file Zanshin_BME680.cpp
    @section Zanshin_BME680_intro_section Description

    ESP-IDF library for the Bosch BME680 sensor\n\n
    See the main library header file for all details
*/

#include "BME680Driver.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/***************************************************************************************************
** Declare class-internal and not publically visible constants
*                  **
***************************************************************************************************/
const uint8_t BME680_STATUS_REGISTER{0x1D};      ///< Device status register
const uint8_t BME680_GAS_HEATER_REGISTER0{0x5A}; ///< Heater Register 0 address
const uint8_t BME680_GAS_DURATION_REGISTER0{
    0x64}; ///< Heater Register 0 address
const uint8_t BME680_CONTROL_GAS_REGISTER1{
    0x70}; ///< Gas control register on/off
const uint8_t BME680_CONTROL_GAS_REGISTER2{
    0x71}; ///< Gas control register settings
const uint8_t BME680_CONTROL_HUMIDITY_REGISTER{
    0x72};                               ///< Humidity control register
const uint8_t BME680_SPI_REGISTER{0x73}; ///< Status register for SPI memory
const uint8_t BME680_CONTROL_MEASURE_REGISTER{
    0x74};                                  ///< Temp, Pressure control register
const uint8_t BME680_CONFIG_REGISTER{0x75}; ///< Configuration register
const uint8_t BME680_CHIPID_REGISTER{0xD0}; ///< Chip-Id register
const uint8_t BME680_SOFTRESET_REGISTER{
    0xE0};                             ///< Reset when 0xB6 is written here
const uint8_t BME680_CHIPID{0x61};     ///< Hard-coded value 0x61 for BME680
const uint8_t BME680_RESET_CODE{0xB6}; ///< Reset when this put in reset reg
const uint8_t BME680_MEASURING_BIT_POSITION{
    5}; ///< Bit position for measuring flag
const uint8_t BME680_I2C_MIN_ADDRESS{
    0x76}; ///< Minimum possible address for BME680
const uint8_t BME680_I2C_MAX_ADDRESS{
    0x77}; ///< Maximum possible address for BME680
const uint8_t BME680_SPI_MEM_PAGE_POSITION{
    4}; ///< Bit position for memory page value
const uint8_t BME680_HUMIDITY_MASK{0xF8};    ///< Mask is binary B11111000
const uint8_t BME680_PRESSURE_MASK{0xE3};    ///< Mask is binary B11100011
const uint8_t BME680_TEMPERATURE_MASK{0x1F}; ///< Mask is binary B00011111
/***************************************************************************************************
** Declare the constants used for calibration                   **
***************************************************************************************************/
const uint8_t BME680_COEFF_SIZE1{25}; ///< First array with coefficients
const uint8_t BME680_COEFF_SIZE2{16}; ///< Second array with coefficients
const uint8_t BME680_COEFF_START_ADDRESS1{0x89}; ///< start address for array 1
const uint8_t BME680_COEFF_START_ADDRESS2{0xE1}; ///< start address for array 2
const uint8_t BME680_HUM_REG_SHIFT_VAL{4};  ///< Ambient humidity shift value
const uint8_t BME680_BIT_H1_DATA_MSK{0x0F}; ///< Mask for humidity
const uint8_t BME680_T2_LSB_REG{1};
const uint8_t BME680_T2_MSB_REG{2};
const uint8_t BME680_T3_REG{3};
const uint8_t BME680_P1_LSB_REG{5};
const uint8_t BME680_P1_MSB_REG{6};
const uint8_t BME680_P2_LSB_REG{7};
const uint8_t BME680_P2_MSB_REG{8};
const uint8_t BME680_P3_REG{9};
const uint8_t BME680_P4_LSB_REG{11};
const uint8_t BME680_P4_MSB_REG{12};
const uint8_t BME680_P5_LSB_REG{13};
const uint8_t BME680_P5_MSB_REG{14};
const uint8_t BME680_P7_REG{15};
const uint8_t BME680_P6_REG{16};
const uint8_t BME680_P8_LSB_REG{19};
const uint8_t BME680_P8_MSB_REG{20};
const uint8_t BME680_P9_LSB_REG{21};
const uint8_t BME680_P9_MSB_REG{22};
const uint8_t BME680_P10_REG{23};
const uint8_t BME680_H2_MSB_REG{0};
const uint8_t BME680_H2_LSB_REG{1};
const uint8_t BME680_H1_LSB_REG{1};
const uint8_t BME680_H1_MSB_REG{2};
const uint8_t BME680_H3_REG{3};
const uint8_t BME680_H4_REG{4};
const uint8_t BME680_H5_REG{5};
const uint8_t BME680_H6_REG{6};
const uint8_t BME680_H7_REG{7};
const uint8_t BME680_T1_LSB_REG{8};
const uint8_t BME680_T1_MSB_REG{9};
const uint8_t BME680_GH2_LSB_REG{10};
const uint8_t BME680_GH2_MSB_REG{11};
const uint8_t BME680_GH1_REG{12};
const uint8_t BME680_GH3_REG{13};
const uint8_t BME680_ADDR_RES_HEAT_RANGE_ADDR{0x02};
const uint8_t BME680_RHRANGE_MSK{0x30};
const uint8_t BME680_ADDR_RES_HEAT_VAL_ADDR{0x00};
const uint8_t BME680_ADDR_RANGE_SW_ERR_ADDR{0x04};
const uint8_t BME680_RSERROR_MSK{0xF0};

BME680Driver::BME680Driver() {
  /*!
   * @brief   Class constructor — all members are zero-initialised in
   * declarations
   */
} // of class constructor

BME680Driver::~BME680Driver() {
  /*!
   * @brief   Class destructor — release the I2C device handle if held
   */
  if (_i2cDevHandle != nullptr) {
    i2c_master_bus_rm_device(_i2cDevHandle);
    _i2cDevHandle = nullptr;
  }
} // of class destructor

bool BME680Driver::begin(i2c_master_bus_handle_t busHandle,
                         const uint8_t i2cAddress) {
  /*!
  @brief   Initialise communications with the BME680 over I2C
  @details Probes I2C addresses 0x76 and 0x77 (or only the specified address
  when i2cAddress != 0). On a successful ACK the device handle is created and
           common initialisation is run.
  @param[in] busHandle  ESP-IDF I2C master bus handle (created by the caller)
  @param[in] i2cAddress Specific I2C address, or 0 to auto-detect
  @return   true on success
  */

  // Release any previously held device handle
  if (_i2cDevHandle != nullptr) {
    i2c_master_bus_rm_device(_i2cDevHandle);
    _i2cDevHandle = nullptr;
  }

  for (_I2CAddress = BME680_I2C_MIN_ADDRESS;
       _I2CAddress <= BME680_I2C_MAX_ADDRESS; _I2CAddress++) {
    if (i2cAddress != 0 && _I2CAddress != i2cAddress)
      continue;

    if (i2c_master_probe(busHandle, _I2CAddress, 50) != ESP_OK)
      continue;

    i2c_device_config_t devConfig = {};
    devConfig.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    devConfig.device_address = _I2CAddress;
    devConfig.scl_speed_hz = I2C_FAST_MODE;

    if (i2c_master_bus_add_device(busHandle, &devConfig, &_i2cDevHandle) ==
        ESP_OK) {
      _I2CSpeed = I2C_FAST_MODE;
      return commonInitialization();
    }
  }

  _I2CAddress = 0;
  return false;
} // of method begin()

bool BME680Driver::commonInitialization() {
  /*!
  @brief   Common BME680 initialisation function
  @details Called from begin() once the I2C device handle is ready.
           Verifies chip ID, loads calibration, and triggers the first
  measurement.
  @return  true on success
  */
  // For I2C mode _I2CAddress is always non-zero so the SPI page-switch
  // branches below are never taken; they are kept for reference only.
  uint8_t SPI_Register = readByte(BME680_SPI_REGISTER);
  if (_I2CAddress == 0 &&
      ((SPI_Register >> BME680_SPI_MEM_PAGE_POSITION) & 1)) {
    SPI_Register &= ~(uint8_t)(1 << BME680_SPI_MEM_PAGE_POSITION);
    putData(BME680_SPI_REGISTER, SPI_Register);
  }

  if (readByte(BME680_CHIPID_REGISTER) == BME680_CHIPID) {
    getCalibration();
    if (_I2CAddress == 0) // SPI only — restore page 1
    {
      SPI_Register |= (uint8_t)(1 << BME680_SPI_MEM_PAGE_POSITION);
      putData(BME680_SPI_REGISTER, SPI_Register);
    }
    uint8_t workRegister = readByte(BME680_CONTROL_MEASURE_REGISTER);
    putData(BME680_CONTROL_MEASURE_REGISTER, (uint8_t)(workRegister | 1));
    return true;
  }
  return false;
} // of method commonInitialization()

uint8_t BME680Driver::readByte(const uint8_t addr) const {
  /*!
  @brief   Read a single byte from the given register address
  @return  Byte read from addr
  */
  uint8_t returnValue = 0;
  getData(addr, returnValue);
  return returnValue;
} // of method readByte()

void BME680Driver::reset() {
  /*!
   * @brief   Performs a device soft reset then re-initialises the chip
   * registers
   */
  putData(BME680_SOFTRESET_REGISTER, BME680_RESET_CODE);
  vTaskDelay(pdMS_TO_TICKS(2)); // datasheet: 2 ms start-up time
  commonInitialization();
} // of method reset()

void BME680Driver::getCalibration() {
  /*!
  @brief    Reads calibration registers into class variables
  */
  uint8_t coeff_arr1[BME680_COEFF_SIZE1] = {0};
  uint8_t coeff_arr2[BME680_COEFF_SIZE2] = {0};
  getData(BME680_COEFF_START_ADDRESS1, coeff_arr1);
  getData(BME680_COEFF_START_ADDRESS2, coeff_arr2);

  _T1 = (uint16_t)(CONCAT_BYTES(coeff_arr2[BME680_T1_MSB_REG],
                                coeff_arr2[BME680_T1_LSB_REG]));
  _T2 = (int16_t)(CONCAT_BYTES(coeff_arr1[BME680_T2_MSB_REG],
                               coeff_arr1[BME680_T2_LSB_REG]));
  _T3 = (int8_t)(coeff_arr1[BME680_T3_REG]);

  _P1 = (uint16_t)(CONCAT_BYTES(coeff_arr1[BME680_P1_MSB_REG],
                                coeff_arr1[BME680_P1_LSB_REG]));
  _P2 = (int16_t)(CONCAT_BYTES(coeff_arr1[BME680_P2_MSB_REG],
                               coeff_arr1[BME680_P2_LSB_REG]));
  _P3 = (int8_t)coeff_arr1[BME680_P3_REG];
  _P4 = (int16_t)(CONCAT_BYTES(coeff_arr1[BME680_P4_MSB_REG],
                               coeff_arr1[BME680_P4_LSB_REG]));
  _P5 = (int16_t)(CONCAT_BYTES(coeff_arr1[BME680_P5_MSB_REG],
                               coeff_arr1[BME680_P5_LSB_REG]));
  _P6 = (int8_t)(coeff_arr1[BME680_P6_REG]);
  _P7 = (int8_t)(coeff_arr1[BME680_P7_REG]);
  _P8 = (int16_t)(CONCAT_BYTES(coeff_arr1[BME680_P8_MSB_REG],
                               coeff_arr1[BME680_P8_LSB_REG]));
  _P9 = (int16_t)(CONCAT_BYTES(coeff_arr1[BME680_P9_MSB_REG],
                               coeff_arr1[BME680_P9_LSB_REG]));
  _P10 = (uint8_t)(coeff_arr1[BME680_P10_REG]);

  _H1 =
      (uint16_t)(((uint16_t)coeff_arr2[BME680_H1_MSB_REG]
                  << BME680_HUM_REG_SHIFT_VAL) |
                 ((coeff_arr2[BME680_H1_LSB_REG] >> BME680_HUM_REG_SHIFT_VAL) &
                  BME680_BIT_H1_DATA_MSK));
  _H2 =
      (uint16_t)(((uint16_t)coeff_arr2[BME680_H2_MSB_REG]
                  << BME680_HUM_REG_SHIFT_VAL) |
                 ((coeff_arr2[BME680_H2_LSB_REG] >> BME680_HUM_REG_SHIFT_VAL) &
                  BME680_BIT_H1_DATA_MSK));
  _H3 = (int8_t)coeff_arr2[BME680_H3_REG];
  _H4 = (int8_t)coeff_arr2[BME680_H4_REG];
  _H5 = (int8_t)coeff_arr2[BME680_H5_REG];
  _H6 = (uint8_t)coeff_arr2[BME680_H6_REG];
  _H7 = (int8_t)coeff_arr2[BME680_H7_REG];

  _G1 = (int8_t)coeff_arr2[BME680_GH1_REG];
  _G2 = (int16_t)(CONCAT_BYTES(coeff_arr2[BME680_GH2_MSB_REG],
                               coeff_arr2[BME680_GH2_LSB_REG]));
  _G3 = (int8_t)coeff_arr2[BME680_GH3_REG];

  uint8_t temp_var = 0;
  getData(BME680_ADDR_RES_HEAT_RANGE_ADDR, temp_var);
  _res_heat_range = ((temp_var & BME680_RHRANGE_MSK) / 16);
  getData(BME680_ADDR_RES_HEAT_VAL_ADDR, temp_var);
  _res_heat = (int8_t)temp_var;
  getData(BME680_ADDR_RANGE_SW_ERR_ADDR, temp_var);
  _rng_sw_err = ((int8_t)temp_var & (int8_t)BME680_RSERROR_MSK) / 16;
} // of method getCalibration()

uint8_t BME680Driver::setOversampling(const uint8_t sensor,
                                      const uint8_t sampling) const {
  /*!
  @brief   Set the oversampling mode for a sensor
  @return  Current sampling value, or UINT8_MAX on error
  */
  if (sensor >= UnknownSensor ||
      (sampling != UINT8_MAX && sampling >= UnknownOversample))
    return UINT8_MAX;

  uint8_t tempRegister;
  uint8_t returnValue = sampling;
  waitForReadings();

  switch (sensor) {
  case HumiditySensor: {
    tempRegister = readByte(BME680_CONTROL_HUMIDITY_REGISTER);
    if (sampling == UINT8_MAX) {
      returnValue = tempRegister & ~BME680_HUMIDITY_MASK;
    } else {
      tempRegister &= BME680_HUMIDITY_MASK;
      tempRegister |= sampling;
      putData(BME680_CONTROL_HUMIDITY_REGISTER, (uint8_t)tempRegister);
    }
    break;
  }
  case PressureSensor: {
    tempRegister = readByte(BME680_CONTROL_MEASURE_REGISTER);
    if (sampling == UINT8_MAX) {
      returnValue = (tempRegister & ~BME680_PRESSURE_MASK) >> 2;
    } else {
      tempRegister &= BME680_PRESSURE_MASK;
      tempRegister |= (sampling << 2);
      putData(BME680_CONTROL_MEASURE_REGISTER, (uint8_t)tempRegister);
    }
    break;
  }
  case TemperatureSensor: {
    tempRegister = readByte(BME680_CONTROL_MEASURE_REGISTER);
    if (sampling == UINT8_MAX) {
      returnValue = (tempRegister & ~BME680_TEMPERATURE_MASK) >> 5;
    } else {
      tempRegister &= BME680_TEMPERATURE_MASK;
      tempRegister |= (sampling << 5);
      putData(BME680_CONTROL_MEASURE_REGISTER, (uint8_t)tempRegister);
    }
    break;
  }
  default:
    return UINT8_MAX;
  }
  return returnValue;
} // of method setOversampling()

uint8_t BME680Driver::setIIRFilter(const uint8_t iirFilterSetting) const {
  /*!
   @brief   Set or return the current IIR filter setting
   @return  IIR Filter setting
   */
  waitForReadings();
  uint8_t returnValue = readByte(BME680_CONFIG_REGISTER);
  if (iirFilterSetting != UINT8_MAX) {
    returnValue = returnValue & 0xE3;              // mask IIR bits (0b11100011)
    returnValue |= (iirFilterSetting & 0x07) << 2; // apply 3-bit setting
    putData(BME680_CONFIG_REGISTER, returnValue);
  }
  returnValue = (returnValue >> 2) & 0x07;
  return returnValue;
} // of method setIIRFilter()

uint8_t BME680Driver::getSensorData(int32_t &temp, int32_t &hum, int32_t &press,
                                    int32_t &gas, const bool waitSwitch) {
  /*!
   @brief   Returns the most recent temperature, humidity, pressure and gas
   readings
   */
  uint8_t status = readSensors(waitSwitch);
  temp = _Temperature;
  hum = _Humidity;
  press = _Pressure;
  gas = _Gas;
  return status;
} // of method getSensorData()

uint8_t BME680Driver::getI2CAddress() const {
  return _I2CAddress;
} // of method getI2CAddress()

uint8_t BME680Driver::readSensors(const bool waitSwitch) {
  /*!
   @brief   Reads all 4 sensor values and converts them to metric units
   */
  const uint32_t lookupTable1[16] = {
      UINT32_C(2147483647), UINT32_C(2147483647), UINT32_C(2147483647),
      UINT32_C(2147483647), UINT32_C(2147483647), UINT32_C(2126008810),
      UINT32_C(2147483647), UINT32_C(2130303777), UINT32_C(2147483647),
      UINT32_C(2147483647), UINT32_C(2143188679), UINT32_C(2136746228),
      UINT32_C(2147483647), UINT32_C(2126008810), UINT32_C(2147483647),
      UINT32_C(2147483647)};
  const uint32_t lookupTable2[16] = {
      UINT32_C(4096000000), UINT32_C(2048000000), UINT32_C(1024000000),
      UINT32_C(512000000),  UINT32_C(255744255),  UINT32_C(127110228),
      UINT32_C(64000000),   UINT32_C(32258064),   UINT32_C(16016016),
      UINT32_C(8000000),    UINT32_C(4000000),    UINT32_C(2000000),
      UINT32_C(1000000),    UINT32_C(500000),     UINT32_C(250000),
      UINT32_C(125000)};

  uint8_t buff[15] = {0};
  uint8_t gas_range = 0;
  int64_t var1, var2, var3, var4, var5, var6, temp_scaled;
  uint32_t adc_temp, adc_pres;
  uint16_t adc_hum, adc_gas_res;

  if (waitSwitch)
    waitForReadings();

  getData(BME680_STATUS_REGISTER, buff);

  adc_pres = (uint32_t)(((uint32_t)buff[2] << 12) | ((uint32_t)buff[3] << 4) |
                        ((uint32_t)buff[4] >> 4));
  adc_temp = (uint32_t)(((uint32_t)buff[5] << 12) | ((uint32_t)buff[6] << 4) |
                        ((uint32_t)buff[7] >> 4));
  adc_hum = (uint16_t)(((uint32_t)buff[8] << 8) | (uint32_t)buff[9]);
  adc_gas_res =
      (uint16_t)((uint32_t)buff[13] << 2 | (((uint32_t)buff[14]) >> 6));
  gas_range = buff[14] & 0x0F;

  // Temperature
  var1 = ((int32_t)adc_temp >> 3) - ((int32_t)_T1 << 1);
  var2 = (var1 * (int32_t)_T2) >> 11;
  var3 = ((var1 >> 1) * (var1 >> 1)) >> 12;
  var3 = ((var3) * ((int32_t)_T3 << 4)) >> 14;
  _tfine = (int32_t)(var2 + var3);
  _Temperature = (int16_t)(((_tfine * 5) + 128) >> 8);

  // Pressure
  var1 = (((int32_t)_tfine) >> 1) - 64000;
  var2 = ((((var1 >> 2) * (var1 >> 2)) >> 11) * (int32_t)_P6) >> 2;
  var2 = var2 + ((var1 * (int32_t)_P5) << 1);
  var2 = (var2 >> 2) + ((int32_t)_P4 << 16);
  var1 = (((((var1 >> 2) * (var1 >> 2)) >> 13) * ((int32_t)_P3 << 5)) >> 3) +
         (((int32_t)_P2 * var1) >> 1);
  var1 = var1 >> 18;
  var1 = ((32768 + var1) * (int32_t)_P1) >> 15;
  _Pressure = 1048576 - adc_pres;
  _Pressure = (int32_t)((_Pressure - (var2 >> 12)) * ((uint32_t)3125));

  if (_Pressure >= INT32_C(0x40000000))
    _Pressure = ((_Pressure / (uint32_t)var1) << 1);
  else
    _Pressure = ((_Pressure << 1) / (uint32_t)var1);

  var1 =
      ((int32_t)_P9 * (int32_t)(((_Pressure >> 3) * (_Pressure >> 3)) >> 13)) >>
      12;
  var2 = ((int32_t)(_Pressure >> 2) * (int32_t)_P8) >> 13;
  var3 = ((int32_t)(_Pressure >> 8) * (int32_t)(_Pressure >> 8) *
          (int32_t)(_Pressure >> 8) * (int32_t)_P10) >>
         17;
  _Pressure =
      (int32_t)(_Pressure) + ((var1 + var2 + var3 + ((int32_t)_P7 << 7)) >> 4);

  // Humidity
  temp_scaled = (((int32_t)_tfine * 5) + 128) >> 8;
  var1 = (int32_t)(adc_hum - ((int32_t)((int32_t)_H1 << 4))) -
         (((temp_scaled * (int32_t)_H3) / ((int32_t)100)) >> 1);
  var2 = ((int32_t)_H2 *
          (((temp_scaled * (int32_t)_H4) / ((int32_t)100)) +
           (((temp_scaled * ((temp_scaled * (int32_t)_H5) / ((int32_t)100))) >>
             6) /
            ((int32_t)100)) +
           (int32_t)(1 << 14))) >>
         10;
  var3 = var1 * var2;
  var4 = (int32_t)_H6 << 7;
  var4 = ((var4) + ((temp_scaled * (int32_t)_H7) / ((int32_t)100))) >> 4;
  var5 = ((var3 >> 14) * (var3 >> 14)) >> 10;
  var6 = (var4 * var5) >> 1;
  _Humidity = (((var3 + var6) >> 10) * ((int32_t)1000)) >> 12;

  if (_Humidity > 100000)
    _Humidity = 100000;
  else if (_Humidity < 0)
    _Humidity = 0;

  // Gas
  uint64_t uvar2;
  var1 = (int64_t)((1340 + (5 * (int64_t)_rng_sw_err)) *
                   ((int64_t)lookupTable1[gas_range])) >>
         16;
  uvar2 =
      (((int64_t)((int64_t)adc_gas_res << 15) - (int64_t)(16777216)) + var1);
  var3 = (((int64_t)lookupTable2[gas_range] * (int64_t)var1) >> 9);
  _Gas = (uint32_t)((var3 + ((int64_t)uvar2 >> 1)) / (int64_t)uvar2);

  triggerMeasurement();
  return (buff[14] & 0x30); // non-zero if gas or heat stabilisation is invalid
} // of method readSensors()

void BME680Driver::waitForReadings() const {
  /*!
   @brief   Blocks until any active measurement has completed.
   @details A short initial delay is required because the BME680 takes a few
            milliseconds to assert the 'measuring' bit after receiving the
            forced-mode trigger. Without it, the fast ESP-IDF I2C bus polls
            the bit before it is set and returns immediately with stale data.
   */
  vTaskDelay(pdMS_TO_TICKS(5)); // let sensor assert the measuring bit
  while (measuring()) {
  }
} // of method waitForReadings()

bool BME680Driver::setGas(uint16_t GasTemp, uint16_t GasMillis) const {
  /*!
   * @brief    Sets the gas heater target temperature and heating duration
   */
  waitForReadings();
  uint8_t gasRegister = readByte(BME680_CONTROL_GAS_REGISTER2);

  if (GasTemp == 0 || GasMillis == 0) {
    putData(BME680_CONTROL_GAS_REGISTER1,
            (uint8_t)0x08); // turn off heater (0b00001000)
    putData(BME680_CONTROL_GAS_REGISTER2,
            (uint8_t)(gasRegister & 0xEF)); // clear run_gas (0b11101111)
  } else {
    putData(BME680_CONTROL_GAS_REGISTER1, (uint8_t)0x00);

    uint8_t heatr_res;
    int32_t var1, var2, var3, var4, var5, heatr_res_x100;

    if (GasTemp < 200)
      GasTemp = 200;
    else if (GasTemp > 400)
      GasTemp = 400;

    var1 = (((int32_t)(_Temperature / 100) * _H3) / 1000) << 8;
    var2 =
        (_G1 + 784) * (((((_G2 + 154009) * GasTemp * 5) / 100) + 3276800) / 10);
    var3 = var1 + (var2 / 2);
    var4 = (var3 / (_res_heat_range + 4));
    var5 = (131 * _res_heat) + 65536;
    heatr_res_x100 = (int32_t)(((var4 / var5) - 250) * 34);
    heatr_res = (uint8_t)((heatr_res_x100 + 50) / 100);
    putData(BME680_GAS_HEATER_REGISTER0, heatr_res);

    uint8_t factor = 0;
    uint8_t durval;
    if (GasMillis >= 0xfc0) {
      durval = 0xff;
    } else {
      while (GasMillis > 0x3F) {
        GasMillis >>= 2;
        factor++;
      }
      durval = (uint8_t)(GasMillis + (factor * 64));
    }

    putData(BME680_CONTROL_GAS_REGISTER1, (uint8_t)0x00);
    putData(BME680_GAS_DURATION_REGISTER0, durval);
    putData(BME680_CONTROL_GAS_REGISTER2,
            (uint8_t)(gasRegister | 0x10)); // set run_gas (0b00010000)
  }
  return true;
} // of method setGas()

bool BME680Driver::measuring() const {
  /*!
   * @brief Returns true if the BME680 is currently performing a measurement
   */
  return (readByte(BME680_STATUS_REGISTER) &
          _BV(BME680_MEASURING_BIT_POSITION)) != 0;
} // of method measuring()

void BME680Driver::triggerMeasurement() const {
  /*!
   * @brief Trigger a new measurement on the BME680
   */
  uint8_t workRegister = readByte(BME680_CONTROL_MEASURE_REGISTER);
  putData(BME680_CONTROL_MEASURE_REGISTER, (uint8_t)(workRegister | 1));
} // of method triggerMeasurement()
