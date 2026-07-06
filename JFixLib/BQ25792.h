#ifndef BQ25792_H
#define BQ25792_H

#ifndef JFIX_EMULATION

#include "I2CWrapper.h"
#include "driver/gpio.h"
#include "globals.h"
#include <string>

#define BQ25792_I2C_ADDRESS 0x6B

class BQ25792 {
public:
  BQ25792(gpio_num_t charge_enable_pin, gpio_num_t int_pin);

  esp_err_t begin();
  esp_err_t initADC();
  esp_err_t enableCharging();
  esp_err_t disableCharging();
  esp_err_t enterShipMode(); // turn off external ship FET (SDRV_CTRL = Ship)
  esp_err_t getChargerStatus0(uint8_t *status = nullptr);
  esp_err_t getChargerStatus1(uint8_t *status = nullptr);
  esp_err_t getBatteryVoltage(uint16_t *voltage_mV = nullptr);
  esp_err_t getVsysVoltage(uint16_t *voltage_mV = nullptr);
  esp_err_t getVac2Voltage(uint16_t *voltage_mV = nullptr);
  esp_err_t getChargeVoltageLimit(uint16_t *chargeVoltageLimit = nullptr);
  esp_err_t getVacOvp(uint8_t *vac_ovp = nullptr);
  esp_err_t printFaults();
  esp_err_t printFaults1();
  void pingWdt();

  bool isBatteryPresent();
  bool isVbusPresent();
  esp_err_t ignoreTemperatureSensor(bool ignore);

  static void update(void *pvParameters);
  enum ChargerStatus {
    NOT_CHARGING,
    TRICKLE_CHARGE,
    PRE_CHARGE,
    FAST_CHARGE_CC_MODE,
    TAPER_CHARGE_CV_MODE,
    RESERVED,
    TOP_OFF_TIMER_ACTIVE_CHARGING,
    CHARGE_TERMINATION_DONE,
  };
  ChargerStatus chargingStatus = NOT_CHARGING;

  std::string getChargingStatusString() {
    switch (chargingStatus) {
    case NOT_CHARGING:
      return "Not Charging";
    case TRICKLE_CHARGE:
      return "Trickle Charge";
    case PRE_CHARGE:
      return "Pre-Charge";
    case FAST_CHARGE_CC_MODE:
      return "Fast Charge (CC Mode)";
    case TAPER_CHARGE_CV_MODE:
      return "Taper Charge (CV Mode)";
    case RESERVED:
      return "Reserved";
    case TOP_OFF_TIMER_ACTIVE_CHARGING:
      return "Top-Off Timer Active Charging";
    case CHARGE_TERMINATION_DONE:
      return "Charge Termination Done";
    default:
      return "Unknown Status";
    }
  }

private:
  gpio_num_t charge_enable_pin;
  gpio_num_t int_pin;
  i2c_master_dev_handle_t dev_handle = NULL;
  uint16_t batteryVoltage_mV = 0;
  uint16_t vac2Voltage_mV = 0;
  uint16_t vsysVoltage_mV = 0;
  uint16_t chargeLimitVoltage_mV = 0;
  uint8_t vacOvp = 0;
  bool batteryPresent = false;
  bool vbusPresent = false;
};

#endif // !JFIX_EMULATION

#endif // BQ25792_H
