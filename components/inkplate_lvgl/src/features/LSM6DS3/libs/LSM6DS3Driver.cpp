/******************************************************************************
LSM6DS3Driver.cpp
LSM6DS3 ESP-IDF Driver (ported from Arduino/SparkFun)

Ported to ESP-IDF new I2C master driver. SPI removed.
******************************************************************************/

#include "LSM6DS3Driver.h"

//****************************************************************************//
//  LSM6DS3Core
//****************************************************************************//
LSM6DS3Core::LSM6DS3Core() : i2c_dev(NULL) {}

void LSM6DS3Core::setI2CHandle(i2c_master_dev_handle_t handle) {
  i2c_dev = handle;
}

status_t LSM6DS3Core::beginCore(void) {
  // Brief startup delay
  vTaskDelay(pdMS_TO_TICKS(10));

  // Check the ID register to determine if the operation was a success.
  uint8_t readCheck;
  readRegister(&readCheck, LSM6DS3_ACC_GYRO_WHO_AM_I_REG);
  if (readCheck != 0x69) {
    return IMU_HW_ERROR;
  }

  return IMU_SUCCESS;
}

status_t LSM6DS3Core::readRegisterRegion(uint8_t *outputPointer, uint8_t offset,
                                         uint8_t length) {
  esp_err_t err = i2c_master_transmit_receive(
      i2c_dev, &offset, 1, outputPointer, length, pdMS_TO_TICKS(100));
  return (err == ESP_OK) ? IMU_SUCCESS : IMU_HW_ERROR;
}

status_t LSM6DS3Core::readRegister(uint8_t *outputPointer, uint8_t offset) {
  return readRegisterRegion(outputPointer, offset, 1);
}

status_t LSM6DS3Core::readRegisterInt16(int16_t *outputPointer,
                                        uint8_t offset) {
  uint8_t myBuffer[2];
  status_t returnError = readRegisterRegion(myBuffer, offset, 2);
  int16_t output = (int16_t)myBuffer[0] | (int16_t)(myBuffer[1] << 8);
  *outputPointer = output;
  return returnError;
}

status_t LSM6DS3Core::writeRegister(uint8_t offset, uint8_t dataToWrite) {
  uint8_t buf[2] = {offset, dataToWrite};
  esp_err_t err = i2c_master_transmit(i2c_dev, buf, 2, pdMS_TO_TICKS(100));
  return (err == ESP_OK) ? IMU_SUCCESS : IMU_HW_ERROR;
}

status_t LSM6DS3Core::embeddedPage(void) {
  return writeRegister(LSM6DS3_ACC_GYRO_RAM_ACCESS, 0x80);
}

status_t LSM6DS3Core::basePage(void) {
  return writeRegister(LSM6DS3_ACC_GYRO_RAM_ACCESS, 0x00);
}

//****************************************************************************//
//  LSM6DS3
//****************************************************************************//
LSM6DS3Driver::LSM6DS3Driver() : LSM6DS3Core() {
  settings.gyroEnabled = 1;
  settings.gyroRange = 2000;
  settings.gyroSampleRate = 416;
  settings.gyroBandWidth = 400;
  settings.gyroFifoEnabled = 1;
  settings.gyroFifoDecimation = 1;

  settings.accelEnabled = 1;
  settings.accelODROff = 1;
  settings.accelRange = 16;
  settings.accelSampleRate = 416;
  settings.accelBandWidth = 100;
  settings.accelFifoEnabled = 1;
  settings.accelFifoDecimation = 1;

  settings.tempEnabled = 1;

  settings.commMode = 1;

  settings.fifoThreshold = 3000;
  settings.fifoSampleRate = 10;
  settings.fifoModeWord = 0;

  allOnesCounter = 0;
  nonSuccessCounter = 0;
}

status_t LSM6DS3Driver::begin(SensorSettings *pSettingsYouWanted) {
  uint8_t dataToWrite = 0;

  status_t returnError = beginCore();

  if (pSettingsYouWanted != NULL) {
    pSettingsYouWanted->gyroEnabled = settings.gyroEnabled;
    pSettingsYouWanted->gyroRange = settings.gyroRange;
    pSettingsYouWanted->gyroSampleRate = settings.gyroSampleRate;
    pSettingsYouWanted->gyroBandWidth = settings.gyroBandWidth;
    pSettingsYouWanted->gyroFifoEnabled = settings.gyroFifoEnabled;
    pSettingsYouWanted->gyroFifoDecimation = settings.gyroFifoDecimation;
    pSettingsYouWanted->accelEnabled = settings.accelEnabled;
    pSettingsYouWanted->accelODROff = settings.accelODROff;
    pSettingsYouWanted->accelRange = settings.accelRange;
    pSettingsYouWanted->accelSampleRate = settings.accelSampleRate;
    pSettingsYouWanted->accelBandWidth = settings.accelBandWidth;
    pSettingsYouWanted->accelFifoEnabled = settings.accelFifoEnabled;
    pSettingsYouWanted->accelFifoDecimation = settings.accelFifoDecimation;
    pSettingsYouWanted->tempEnabled = settings.tempEnabled;
    pSettingsYouWanted->commMode = settings.commMode;
    pSettingsYouWanted->fifoThreshold = settings.fifoThreshold;
    pSettingsYouWanted->fifoSampleRate = settings.fifoSampleRate;
    pSettingsYouWanted->fifoModeWord = settings.fifoModeWord;
  }

  // Setup accelerometer (CTRL1_XL)
  dataToWrite = 0;
  if (settings.accelEnabled == 1) {
    switch (settings.accelBandWidth) {
    case 50:
      dataToWrite |= LSM6DS3_ACC_GYRO_BW_XL_50Hz;
      break;
    case 100:
      dataToWrite |= LSM6DS3_ACC_GYRO_BW_XL_100Hz;
      break;
    case 200:
      dataToWrite |= LSM6DS3_ACC_GYRO_BW_XL_200Hz;
      break;
    default:
      settings.accelBandWidth = 400;
      dataToWrite |= LSM6DS3_ACC_GYRO_BW_XL_400Hz;
      break;
    case 400:
      dataToWrite |= LSM6DS3_ACC_GYRO_BW_XL_400Hz;
      break;
    }
    switch (settings.accelRange) {
    case 2:
      dataToWrite |= LSM6DS3_ACC_GYRO_FS_XL_2g;
      break;
    case 4:
      dataToWrite |= LSM6DS3_ACC_GYRO_FS_XL_4g;
      break;
    case 8:
      dataToWrite |= LSM6DS3_ACC_GYRO_FS_XL_8g;
      break;
    default:
      settings.accelRange = 16;
      dataToWrite |= LSM6DS3_ACC_GYRO_FS_XL_16g;
      break;
    case 16:
      dataToWrite |= LSM6DS3_ACC_GYRO_FS_XL_16g;
      break;
    }
    switch (settings.accelSampleRate) {
    case 13:
      dataToWrite |= LSM6DS3_ACC_GYRO_ODR_XL_13Hz;
      break;
    case 26:
      dataToWrite |= LSM6DS3_ACC_GYRO_ODR_XL_26Hz;
      break;
    case 52:
      dataToWrite |= LSM6DS3_ACC_GYRO_ODR_XL_52Hz;
      break;
    default:
      settings.accelSampleRate = 104;
      dataToWrite |= LSM6DS3_ACC_GYRO_ODR_XL_104Hz;
      break;
    case 104:
      dataToWrite |= LSM6DS3_ACC_GYRO_ODR_XL_104Hz;
      break;
    case 208:
      dataToWrite |= LSM6DS3_ACC_GYRO_ODR_XL_208Hz;
      break;
    case 416:
      dataToWrite |= LSM6DS3_ACC_GYRO_ODR_XL_416Hz;
      break;
    case 833:
      dataToWrite |= LSM6DS3_ACC_GYRO_ODR_XL_833Hz;
      break;
    case 1660:
      dataToWrite |= LSM6DS3_ACC_GYRO_ODR_XL_1660Hz;
      break;
    case 3330:
      dataToWrite |= LSM6DS3_ACC_GYRO_ODR_XL_3330Hz;
      break;
    case 6660:
      dataToWrite |= LSM6DS3_ACC_GYRO_ODR_XL_6660Hz;
      break;
    case 13330:
      dataToWrite |= LSM6DS3_ACC_GYRO_ODR_XL_13330Hz;
      break;
    }
  }
  writeRegister(LSM6DS3_ACC_GYRO_CTRL1_XL, dataToWrite);

  // Set the ODR bit (CTRL4_C)
  readRegister(&dataToWrite, LSM6DS3_ACC_GYRO_CTRL4_C);
  dataToWrite &= ~((uint8_t)LSM6DS3_ACC_GYRO_BW_SCAL_ODR_ENABLED);
  if (settings.accelODROff == 1) {
    dataToWrite |= LSM6DS3_ACC_GYRO_BW_SCAL_ODR_ENABLED;
  }
  writeRegister(LSM6DS3_ACC_GYRO_CTRL4_C, dataToWrite);

  // Setup gyroscope (CTRL2_G)
  dataToWrite = 0;
  if (settings.gyroEnabled == 1) {
    switch (settings.gyroRange) {
    case 125:
      dataToWrite |= LSM6DS3_ACC_GYRO_FS_125_ENABLED;
      break;
    case 245:
      dataToWrite |= LSM6DS3_ACC_GYRO_FS_G_245dps;
      break;
    case 500:
      dataToWrite |= LSM6DS3_ACC_GYRO_FS_G_500dps;
      break;
    case 1000:
      dataToWrite |= LSM6DS3_ACC_GYRO_FS_G_1000dps;
      break;
    default:
      settings.gyroRange = 2000;
      dataToWrite |= LSM6DS3_ACC_GYRO_FS_G_2000dps;
      break;
    case 2000:
      dataToWrite |= LSM6DS3_ACC_GYRO_FS_G_2000dps;
      break;
    }
    switch (settings.gyroSampleRate) {
    case 13:
      dataToWrite |= LSM6DS3_ACC_GYRO_ODR_G_13Hz;
      break;
    case 26:
      dataToWrite |= LSM6DS3_ACC_GYRO_ODR_G_26Hz;
      break;
    case 52:
      dataToWrite |= LSM6DS3_ACC_GYRO_ODR_G_52Hz;
      break;
    default:
      settings.gyroSampleRate = 104;
      dataToWrite |= LSM6DS3_ACC_GYRO_ODR_G_104Hz;
      break;
    case 104:
      dataToWrite |= LSM6DS3_ACC_GYRO_ODR_G_104Hz;
      break;
    case 208:
      dataToWrite |= LSM6DS3_ACC_GYRO_ODR_G_208Hz;
      break;
    case 416:
      dataToWrite |= LSM6DS3_ACC_GYRO_ODR_G_416Hz;
      break;
    case 833:
      dataToWrite |= LSM6DS3_ACC_GYRO_ODR_G_833Hz;
      break;
    case 1660:
      dataToWrite |= LSM6DS3_ACC_GYRO_ODR_G_1660Hz;
      break;
    }
  }
  writeRegister(LSM6DS3_ACC_GYRO_CTRL2_G, dataToWrite);

  // Warm-up reads: sensor needs a few reads before outputting stable values
  for (int i = 0; i < 3; i++) {
    readFloatAccelX();
    readFloatAccelY();
    readFloatAccelZ();
    readFloatGyroX();
    readFloatGyroY();
    readFloatGyroZ();
    vTaskDelay(pdMS_TO_TICKS(100));
  }

  return returnError;
}

//****************************************************************************//
//  Accelerometer
//****************************************************************************//
int16_t LSM6DS3Driver::readRawAccelX(void) {
  int16_t output;
  status_t errorLevel = readRegisterInt16(&output, LSM6DS3_ACC_GYRO_OUTX_L_XL);
  if (errorLevel != IMU_SUCCESS) {
    if (errorLevel == IMU_ALL_ONES_WARNING)
      allOnesCounter++;
    else
      nonSuccessCounter++;
  }
  return output;
}
float LSM6DS3Driver::readFloatAccelX(void) {
  return calcAccel(readRawAccelX());
}

int16_t LSM6DS3Driver::readRawAccelY(void) {
  int16_t output;
  status_t errorLevel = readRegisterInt16(&output, LSM6DS3_ACC_GYRO_OUTY_L_XL);
  if (errorLevel != IMU_SUCCESS) {
    if (errorLevel == IMU_ALL_ONES_WARNING)
      allOnesCounter++;
    else
      nonSuccessCounter++;
  }
  return output;
}
float LSM6DS3Driver::readFloatAccelY(void) {
  return calcAccel(readRawAccelY());
}

int16_t LSM6DS3Driver::readRawAccelZ(void) {
  int16_t output;
  status_t errorLevel = readRegisterInt16(&output, LSM6DS3_ACC_GYRO_OUTZ_L_XL);
  if (errorLevel != IMU_SUCCESS) {
    if (errorLevel == IMU_ALL_ONES_WARNING)
      allOnesCounter++;
    else
      nonSuccessCounter++;
  }
  return output;
}
float LSM6DS3Driver::readFloatAccelZ(void) {
  return calcAccel(readRawAccelZ());
}

float LSM6DS3Driver::calcAccel(int16_t input) {
  return (float)input * 0.061f * (settings.accelRange >> 1) / 1000.0f;
}

//****************************************************************************//
//  Gyroscope
//****************************************************************************//
int16_t LSM6DS3Driver::readRawGyroX(void) {
  int16_t output;
  status_t errorLevel = readRegisterInt16(&output, LSM6DS3_ACC_GYRO_OUTX_L_G);
  if (errorLevel != IMU_SUCCESS) {
    if (errorLevel == IMU_ALL_ONES_WARNING)
      allOnesCounter++;
    else
      nonSuccessCounter++;
  }
  return output;
}
float LSM6DS3Driver::readFloatGyroX(void) { return calcGyro(readRawGyroX()); }

int16_t LSM6DS3Driver::readRawGyroY(void) {
  int16_t output;
  status_t errorLevel = readRegisterInt16(&output, LSM6DS3_ACC_GYRO_OUTY_L_G);
  if (errorLevel != IMU_SUCCESS) {
    if (errorLevel == IMU_ALL_ONES_WARNING)
      allOnesCounter++;
    else
      nonSuccessCounter++;
  }
  return output;
}
float LSM6DS3Driver::readFloatGyroY(void) { return calcGyro(readRawGyroY()); }

int16_t LSM6DS3Driver::readRawGyroZ(void) {
  int16_t output;
  status_t errorLevel = readRegisterInt16(&output, LSM6DS3_ACC_GYRO_OUTZ_L_G);
  if (errorLevel != IMU_SUCCESS) {
    if (errorLevel == IMU_ALL_ONES_WARNING)
      allOnesCounter++;
    else
      nonSuccessCounter++;
  }
  return output;
}
float LSM6DS3Driver::readFloatGyroZ(void) { return calcGyro(readRawGyroZ()); }

float LSM6DS3Driver::calcGyro(int16_t input) {
  uint8_t gyroRangeDivisor = settings.gyroRange / 125;
  if (settings.gyroRange == 245)
    gyroRangeDivisor = 2;
  return (float)input * 4.375f * gyroRangeDivisor / 1000.0f;
}

//****************************************************************************//
//  Temperature
//****************************************************************************//
int16_t LSM6DS3Driver::readRawTemp(void) {
  int16_t output;
  readRegisterInt16(&output, LSM6DS3_ACC_GYRO_OUT_TEMP_L);
  return output;
}

float LSM6DS3Driver::readTempC(void) {
  return (float)readRawTemp() / 16.0f + 25.0f;
}

float LSM6DS3Driver::readTempF(void) {
  return readTempC() * 9.0f / 5.0f + 32.0f;
}

//****************************************************************************//
//  FIFO
//****************************************************************************//
void LSM6DS3Driver::fifoBegin(void) {
  uint8_t thresholdLByte = settings.fifoThreshold & 0x00FF;
  uint8_t thresholdHByte = (settings.fifoThreshold & 0x0F00) >> 8;

  uint8_t tempFIFO_CTRL3 = 0;
  if (settings.gyroFifoEnabled == 1)
    tempFIFO_CTRL3 |= (settings.gyroFifoDecimation & 0x07) << 3;
  if (settings.accelFifoEnabled == 1)
    tempFIFO_CTRL3 |= (settings.accelFifoDecimation & 0x07);

  uint8_t tempFIFO_CTRL4 = 0;

  uint8_t tempFIFO_CTRL5 = 0;
  switch (settings.fifoSampleRate) {
  default:
  case 10:
    tempFIFO_CTRL5 |= LSM6DS3_ACC_GYRO_ODR_FIFO_10Hz;
    break;
  case 25:
    tempFIFO_CTRL5 |= LSM6DS3_ACC_GYRO_ODR_FIFO_25Hz;
    break;
  case 50:
    tempFIFO_CTRL5 |= LSM6DS3_ACC_GYRO_ODR_FIFO_50Hz;
    break;
  case 100:
    tempFIFO_CTRL5 |= LSM6DS3_ACC_GYRO_ODR_FIFO_100Hz;
    break;
  case 200:
    tempFIFO_CTRL5 |= LSM6DS3_ACC_GYRO_ODR_FIFO_200Hz;
    break;
  case 400:
    tempFIFO_CTRL5 |= LSM6DS3_ACC_GYRO_ODR_FIFO_400Hz;
    break;
  case 800:
    tempFIFO_CTRL5 |= LSM6DS3_ACC_GYRO_ODR_FIFO_800Hz;
    break;
  case 1600:
    tempFIFO_CTRL5 |= LSM6DS3_ACC_GYRO_ODR_FIFO_1600Hz;
    break;
  case 3300:
    tempFIFO_CTRL5 |= LSM6DS3_ACC_GYRO_ODR_FIFO_3300Hz;
    break;
  case 6600:
    tempFIFO_CTRL5 |= LSM6DS3_ACC_GYRO_ODR_FIFO_6600Hz;
    break;
  }
  tempFIFO_CTRL5 |= settings.fifoModeWord = 6;

  writeRegister(LSM6DS3_ACC_GYRO_FIFO_CTRL1, thresholdLByte);
  writeRegister(LSM6DS3_ACC_GYRO_FIFO_CTRL2, thresholdHByte);
  writeRegister(LSM6DS3_ACC_GYRO_FIFO_CTRL3, tempFIFO_CTRL3);
  writeRegister(LSM6DS3_ACC_GYRO_FIFO_CTRL4, tempFIFO_CTRL4);
  writeRegister(LSM6DS3_ACC_GYRO_FIFO_CTRL5, tempFIFO_CTRL5);
}

void LSM6DS3Driver::fifoClear(void) {
  while ((fifoGetStatus() & 0x1000) == 0)
    fifoRead();
}

int16_t LSM6DS3Driver::fifoRead(void) {
  uint8_t tempReadByte = 0;
  uint16_t tempAccumulator = 0;
  readRegister(&tempReadByte, LSM6DS3_ACC_GYRO_FIFO_DATA_OUT_L);
  tempAccumulator = tempReadByte;
  readRegister(&tempReadByte, LSM6DS3_ACC_GYRO_FIFO_DATA_OUT_H);
  tempAccumulator |= ((uint16_t)tempReadByte << 8);
  return tempAccumulator;
}

uint16_t LSM6DS3Driver::fifoGetStatus(void) {
  uint8_t tempReadByte = 0;
  uint16_t tempAccumulator = 0;
  readRegister(&tempReadByte, LSM6DS3_ACC_GYRO_FIFO_STATUS1);
  tempAccumulator = tempReadByte;
  readRegister(&tempReadByte, LSM6DS3_ACC_GYRO_FIFO_STATUS2);
  tempAccumulator |= (tempReadByte << 8);
  return tempAccumulator;
}

void LSM6DS3Driver::fifoEnd(void) {
  writeRegister(LSM6DS3_ACC_GYRO_FIFO_CTRL5, 0x00);
}
