#ifndef BQ25792_H
#define BQ25792_H

#include "I2CWrapper.h"
#include "driver/gpio.h"

#define BQ25792_I2C_ADDRESS 0x6B

class BQ25792 {
public:
  BQ25792(gpio_num_t charge_enable_pin, gpio_num_t int_pin);

  esp_err_t begin();
  esp_err_t enableCharging();
  esp_err_t disableCharging();
  esp_err_t getChargerStatus(uint8_t *status);

private:
  gpio_num_t charge_enable_pin;
  gpio_num_t int_pin;
  i2c_master_dev_handle_t dev_handle = NULL;
};

#endif // BQ25792_H
