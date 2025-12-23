#include "BQ25792.h"

// Example Register
#define BQ25792_CHARGER_STATUS_REG 0x21

BQ25792::BQ25792(gpio_num_t charge_enable_pin, gpio_num_t int_pin)
    : charge_enable_pin(charge_enable_pin), int_pin(int_pin) {}

esp_err_t BQ25792::begin() {
  gpio_config_t ce_gpio_config = {
      .pin_bit_mask = (1ULL << charge_enable_pin),
      .mode = GPIO_MODE_OUTPUT,
      .pull_up_en = GPIO_PULLUP_DISABLE,
      .pull_down_en = GPIO_PULLDOWN_ENABLE,
      .intr_type = GPIO_INTR_DISABLE,
  };
  esp_err_t err = gpio_config(&ce_gpio_config);
  if (err != ESP_OK) {
    return err;
  }

  gpio_config_t int_gpio_config = {
      .pin_bit_mask = (1ULL << int_pin),
      .mode = GPIO_MODE_INPUT,
      .pull_up_en = GPIO_PULLUP_ENABLE,
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .intr_type = GPIO_INTR_NEGEDGE,
  };
  err = gpio_config(&int_gpio_config);

  if (err == ESP_OK) {
    err = i2c.addDevice(BQ25792_I2C_ADDRESS, 1000000, &dev_handle);
  }

  if (err == ESP_OK) {
    err = disableCharging();
  }

  return err;
}

esp_err_t BQ25792::enableCharging() {
  return gpio_set_level(charge_enable_pin, 1);
}

esp_err_t BQ25792::disableCharging() {
  return gpio_set_level(charge_enable_pin, 0);
}

esp_err_t BQ25792::getChargerStatus(uint8_t *status) {
  if (dev_handle == NULL)
    return ESP_ERR_INVALID_STATE;
  // Read the charger status register
  return I2CWrapper::read(dev_handle, BQ25792_CHARGER_STATUS_REG, status, 1);
}
