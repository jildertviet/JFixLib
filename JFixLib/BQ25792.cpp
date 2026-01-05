#include "BQ25792.h"

// Example Register
#define BQ25792_CHARGER_CONTROL_5_REG 0x14
#define BQ25792_NTC_CONTROL_1_REG 0x18
#define BQ25792_CHARGER_STATUS_REG 0x1B
#define BQ25792_CHARGER_STATUS_2_REG 0x1D
#define BQ25792_ADC_CONTROL_REG 0x2E
#define BQ25792_VSYS_ADC_REG 0x3D

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
    // REG14_Charger_Control_5: SFET_PRESENT=1 (bit 7), rest default (0x16)
    uint8_t ctrl5 = 0x96;
    err =
        I2CWrapper::write(dev_handle, BQ25792_CHARGER_CONTROL_5_REG, &ctrl5, 1);
  }

  if (err == ESP_OK) {
    err = initADC();
  }

  if (err == ESP_OK) {
    err = disableCharging();
  }

  ignoreTemperatureSensor(true);

  xTaskCreate(update, "BQ25792::update", 2048, this, 0, NULL);
  return err;
}

void BQ25792::update(void *pvParameters) {
  BQ25792 *charger = static_cast<BQ25792 *>(pvParameters);
  while (1) {
    if (charger->dev_handle == NULL) {
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      continue;
    }
    charger->getBatteryVoltage();
    charger->getVsysVoltage();
    charger->isBatteryPresent();
    charger->isVbusPresent();
    ESP_LOGI("Charger", "VBAT: %d mV, VSYS: %d mV, BatPres: %s, VbusPres: %s",
             charger->batteryVoltage_mV, charger->vsysVoltage_mV,
             charger->batteryPresent ? "Yes" : "No",
             charger->vbusPresent ? "Yes" : "No");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

esp_err_t BQ25792::initADC() {
  // REG2E_ADC_Control: ADC_EN=1 (bit 7), ADC_RATE=0 (continuous, bit 6),
  // ADC_SAMPLE=00 (15-bit, bits 5-4), ADC_AVG=0, ADC_AVG_INIT=0
  uint8_t adc_ctrl = 0x80;
  return I2CWrapper::write(dev_handle, BQ25792_ADC_CONTROL_REG, &adc_ctrl, 1);
}

esp_err_t BQ25792::enableCharging() {
  return gpio_set_level(charge_enable_pin, 1);
}

esp_err_t BQ25792::disableCharging() {
  return gpio_set_level(charge_enable_pin, 0);
}

esp_err_t BQ25792::getChargerStatus(uint8_t *status) {
  return I2CWrapper::read(dev_handle, BQ25792_CHARGER_STATUS_REG, status, 1);
}

esp_err_t BQ25792::getBatteryVoltage(uint8_t *status) {
  if (status == nullptr) {
    status = (uint8_t *)&batteryVoltage_mV; // Default
  }
  return I2CWrapper::read(dev_handle, BQ25792_CHARGER_STATUS_REG, status, 2);
}

esp_err_t BQ25792::getVsysVoltage(uint16_t *voltage_mV) {
  uint8_t buf[2] = {0};
  esp_err_t err = I2CWrapper::read(dev_handle, BQ25792_VSYS_ADC_REG, buf, 2);
  if (err == ESP_OK) {
    uint16_t val = (buf[0] << 8) | buf[1];
    if (voltage_mV != nullptr) {
      *voltage_mV = val;
    }
    vsysVoltage_mV = val;
  }
  return err;
}

bool BQ25792::isBatteryPresent() {
  uint8_t status;
  if (I2CWrapper::read(dev_handle, BQ25792_CHARGER_STATUS_2_REG, &status, 1) ==
      ESP_OK) {
    batteryPresent = (status & 0x01);
  }
  return batteryPresent;
}

bool BQ25792::isVbusPresent() {
  uint8_t status;
  if (I2CWrapper::read(dev_handle, BQ25792_CHARGER_STATUS_REG, &status, 1) ==
      ESP_OK) {
    vbusPresent = (status & 0x01);
  }
  return vbusPresent;
}

esp_err_t BQ25792::ignoreTemperatureSensor(bool ignore) {
  uint8_t val;
  esp_err_t err =
      I2CWrapper::read(dev_handle, BQ25792_NTC_CONTROL_1_REG, &val, 1);
  if (err != ESP_OK) {
    return err;
  }
  if (ignore) {
    val |= 0x01;
  } else {
    val &= ~0x01;
  }
  return I2CWrapper::write(dev_handle, BQ25792_NTC_CONTROL_1_REG, &val, 1);
}
