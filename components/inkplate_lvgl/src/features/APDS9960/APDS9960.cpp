#include "APDS9960.h"
#include "driver/i2c_master.h"

bool APDS9960::begin(i2c_master_bus_handle_t bus_handle) {
  i2c_device_config_t dev_config = {};
  dev_config.dev_addr_length = I2C_ADDR_BIT_LEN_7;
  dev_config.device_address = APDS9960_I2C_ADDR;
  dev_config.scl_speed_hz = 400000;

  if (i2c_master_bus_add_device(bus_handle, &dev_config, &dev_handle_) !=
      ESP_OK)
    return false;

  return APDS9960Driver::init();
}
