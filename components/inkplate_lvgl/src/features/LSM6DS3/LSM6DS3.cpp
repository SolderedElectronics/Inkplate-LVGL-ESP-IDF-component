#include "LSM6DS3.h"

status_t LSM6DS3::begin(i2c_master_bus_handle_t bus_handle, uint8_t addr) {
  i2c_device_config_t dev_cfg = {};
  dev_cfg.dev_addr_length = I2C_ADDR_BIT_LEN_7;
  dev_cfg.device_address = addr;
  dev_cfg.scl_speed_hz = 400000;

  if (i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle) != ESP_OK)
    return IMU_HW_ERROR;

  setI2CHandle(dev_handle);

  return LSM6DS3Driver::begin();
}
