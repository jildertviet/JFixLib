#include "I2CWrapper.h"
#include "globals.h"
#include <cstring>

I2CWrapper i2c(I2C_PORT, I2C_SDA_PIN, I2C_SCL_PIN);

I2CWrapper::I2CWrapper(i2c_port_num_t port, gpio_num_t sda_pin,
                       gpio_num_t scl_pin) {
  i2c_master_bus_config_t i2c_mst_config = {
      .i2c_port = port,
      .sda_io_num = sda_pin,
      .scl_io_num = scl_pin,
      .clk_source = I2C_CLK_SRC_DEFAULT,
      .glitch_ignore_cnt = 7,
      .intr_priority = 0,
      .trans_queue_depth = 0, // 0 means default
      .flags =
          {
              .enable_internal_pullup = 1,
          },
  };

  // Initialize the I2C master bus
  // Note: Error handling in constructor is limited, usually we might throw or
  // log. Ideally, we'd have an init() method returning esp_err_t. For now, we
  // assume success or user checks logs.
  ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));
}

esp_err_t I2CWrapper::isDevicePresent(uint8_t device_address, int timeout_ms) {
  // This helper function sends a signal to the address and waits for an ACK
  return i2c_master_probe(bus_handle, device_address, timeout_ms);
}

esp_err_t I2CWrapper::addDevice(uint8_t device_address, uint32_t device_speed,
                                i2c_master_dev_handle_t *dev_handle) {
  i2c_device_config_t dev_cfg = {
      .dev_addr_length = I2C_ADDR_BIT_LEN_7,
      .device_address = device_address,
      .scl_speed_hz = device_speed,
  };

  return i2c_master_bus_add_device(bus_handle, &dev_cfg, dev_handle);
}

esp_err_t I2CWrapper::read(i2c_master_dev_handle_t dev_handle, uint8_t reg_addr,
                           uint8_t *data, size_t len) {
  return i2c_master_transmit_receive(dev_handle, &reg_addr, 1, data, len, -1);
}

esp_err_t I2CWrapper::write(i2c_master_dev_handle_t dev_handle,
                            uint8_t reg_addr, const uint8_t *data, size_t len) {
  // Need to send [RegAddr] + [Data...] in one transaction
  // To avoid malloc, we can use a small buffer if len is small, or allocate.
  // For general purpose, let's allocate a temporary buffer.

  // Optimization: If len is small, use stack buffer.
  uint8_t buffer[64];
  uint8_t *tx_buf = buffer;
  bool allocated = false;

  if (len + 1 > sizeof(buffer)) {
    tx_buf = (uint8_t *)malloc(len + 1);
    if (!tx_buf)
      return ESP_ERR_NO_MEM;
    allocated = true;
  }

  tx_buf[0] = reg_addr;
  memcpy(tx_buf + 1, data, len);

  esp_err_t ret = i2c_master_transmit(dev_handle, tx_buf, len + 1, -1);

  if (allocated) {
    free(tx_buf);
  }

  return ret;
}
