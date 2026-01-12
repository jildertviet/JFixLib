#ifndef BQ25792_H
#define BQ25792_H

#include "I2CWrapper.h"
#include "driver/gpio.h"
#include "globals.h"

#define BQ25792_I2C_ADDRESS 0x6B

class BQ25792 {
public:
  BQ25792(gpio_num_t charge_enable_pin, gpio_num_t int_pin);

  esp_err_t begin();
  esp_err_t initADC();
  esp_err_t enableCharging();
  esp_err_t disableCharging();
  esp_err_t getChargerStatus(uint8_t *status);
  esp_err_t getBatteryVoltage(uint8_t *voltage_mV = nullptr);
  esp_err_t getVsysVoltage(uint16_t *voltage_mV = nullptr);
  esp_err_t getVac2Voltage(uint16_t *voltage_mV = nullptr);
  esp_err_t getChargeVoltageLimit(uint16_t *chargeVoltageLimit = nullptr);
  esp_err_t printFaults();

  bool isBatteryPresent();
  bool isVbusPresent();
  esp_err_t ignoreTemperatureSensor(bool ignore);

  static void update(void *pvParameters);

private:
  gpio_num_t charge_enable_pin;
  gpio_num_t int_pin;
  i2c_master_dev_handle_t dev_handle = NULL;
  uint16_t batteryVoltage_mV = 0;
  uint16_t vac2Voltage_mV = 0;
  uint16_t vsysVoltage_mV = 0;
  uint16_t chargeLimitVoltage_mV = 0;
  bool batteryPresent = false;
  bool vbusPresent = false;
};

#endif // BQ25792_H
