#include "MMA8451.h"
#include "esp_log.h"

// Register map
#define MMA8451_REG_STATUS       0x00
#define MMA8451_REG_OUT_X_MSB    0x01
#define MMA8451_REG_WHO_AM_I     0x0D
#define MMA8451_REG_XYZ_DATA_CFG 0x0E
#define MMA8451_REG_CTRL_REG1    0x2A

#define MMA8451_WHO_AM_I_VALUE 0x1A

// CTRL_REG1: ODR = 50 Hz (bits 5:3 = 010), low noise off, fast-read off
#define MMA8451_ODR_50HZ 0x10  // DR[2:0] = 010 → 50 Hz, ACTIVE=0

static const char *TAG = "MMA8451";

MMA8451::MMA8451(uint8_t addr) : _addr(addr) {}

esp_err_t MMA8451::standby() {
  uint8_t val;
  esp_err_t err = I2CWrapper::read(_dev_handle, MMA8451_REG_CTRL_REG1, &val, 1);
  if (err != ESP_OK) return err;
  val &= ~0x01; // clear ACTIVE bit
  return I2CWrapper::write(_dev_handle, MMA8451_REG_CTRL_REG1, &val, 1);
}

esp_err_t MMA8451::active() {
  uint8_t val;
  esp_err_t err = I2CWrapper::read(_dev_handle, MMA8451_REG_CTRL_REG1, &val, 1);
  if (err != ESP_OK) return err;
  val |= 0x01; // set ACTIVE bit
  return I2CWrapper::write(_dev_handle, MMA8451_REG_CTRL_REG1, &val, 1);
}

esp_err_t MMA8451::begin(Range range) {
  esp_err_t err = i2c.addDevice(_addr, 400000, &_dev_handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "addDevice failed: %s", esp_err_to_name(err));
    return err;
  }

  err = i2c.isDevicePresent(_addr, 1000);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "MMA8451 not found at 0x%02X: %s", _addr, esp_err_to_name(err));
    _dev_handle = NULL;
    return err;
  }

  // Verify WHO_AM_I
  uint8_t who = 0;
  err = I2CWrapper::read(_dev_handle, MMA8451_REG_WHO_AM_I, &who, 1);
  if (err != ESP_OK) return err;
  if (who != MMA8451_WHO_AM_I_VALUE) {
    ESP_LOGE(TAG, "WHO_AM_I mismatch: got 0x%02X, expected 0x%02X", who, MMA8451_WHO_AM_I_VALUE);
    return ESP_ERR_NOT_FOUND;
  }

  // Must be in standby to configure
  err = standby();
  if (err != ESP_OK) return err;

  // Set full-scale range
  uint8_t cfg = (uint8_t)range & 0x03;
  err = I2CWrapper::write(_dev_handle, MMA8451_REG_XYZ_DATA_CFG, &cfg, 1);
  if (err != ESP_OK) return err;

  switch (range) {
  case Range::G2: _scale = 1.f / 4096.f; break;
  case Range::G4: _scale = 1.f / 2048.f; break;
  case Range::G8: _scale = 1.f / 1024.f; break;
  }

  // Set ODR to 50 Hz, low-noise off
  uint8_t ctrl1 = MMA8451_ODR_50HZ;
  err = I2CWrapper::write(_dev_handle, MMA8451_REG_CTRL_REG1, &ctrl1, 1);
  if (err != ESP_OK) return err;

  err = active();
  if (err == ESP_OK) {
    ESP_LOGI(TAG, "MMA8451 initialized (range=±%dg, ODR=50Hz)",
             1 << (1 + (int)range));
  }
  return err;
}

esp_err_t MMA8451::read(AccelData &out) {
  if (_dev_handle == NULL) return ESP_ERR_INVALID_STATE;

  // At 50 Hz ODR the registers are always valid; no spin-wait needed when
  // calling at the same rate as the update loop.
  uint8_t buf[6] = {0};
  esp_err_t err = I2CWrapper::read(_dev_handle, MMA8451_REG_OUT_X_MSB, buf, 6);
  if (err != ESP_OK) return err;

  // 14-bit signed, left-justified → arithmetic right-shift by 2
  int16_t raw_x = (int16_t)((buf[0] << 8) | buf[1]) >> 2;
  int16_t raw_y = (int16_t)((buf[2] << 8) | buf[3]) >> 2;
  int16_t raw_z = (int16_t)((buf[4] << 8) | buf[5]) >> 2;

  out.x = raw_x * _scale;
  out.y = raw_y * _scale;
  out.z = raw_z * _scale;
  data = out;

  return ESP_OK;
}
