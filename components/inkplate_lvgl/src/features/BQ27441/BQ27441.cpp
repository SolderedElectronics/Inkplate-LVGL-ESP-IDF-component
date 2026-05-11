#include "BQ27441.h"
#include "driver/i2c_master.h"

bool BQ27441::begin(i2c_master_bus_handle_t bus_handle, uint32_t speed) {
  i2c_device_config_t dev_cfg = {};
  dev_cfg.dev_addr_length = I2C_ADDR_BIT_LEN_7;
  dev_cfg.device_address = BQ72441_I2C_ADDRESS;
  dev_cfg.scl_speed_hz = speed;

  i2c_master_dev_handle_t dev_handle;
  if (i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle) != ESP_OK)
    return false;

  return BQ27441Driver::begin(dev_handle);
}
