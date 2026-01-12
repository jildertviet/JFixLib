#ifndef I2C_WRAPPER_H
#define I2C_WRAPPER_H

#include "driver/i2c_master.h"
#include "esp_err.h"

#define I2C_PORT 0

class I2CWrapper {
public:
  I2CWrapper(i2c_port_num_t port, gpio_num_t sda_pin, gpio_num_t scl_pin);

  esp_err_t addDevice(uint8_t device_address, uint32_t device_speed,
                      i2c_master_dev_handle_t *dev_handle);

  // Helper static methods for common register operations using the device
  // handle
  static esp_err_t read(i2c_master_dev_handle_t dev_handle, uint8_t reg_addr,
                        uint8_t *data, size_t len);
  static esp_err_t write(i2c_master_dev_handle_t dev_handle, uint8_t reg_addr,
                         const uint8_t *data, size_t len);

  esp_err_t isDevicePresent(uint8_t device_address, int timeout_ms);

private:
  i2c_master_bus_handle_t bus_handle;
};

extern I2CWrapper i2c;

#endif // I2C_WRAPPER_H
