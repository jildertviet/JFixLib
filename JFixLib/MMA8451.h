#ifndef MMA8451_H
#define MMA8451_H

#include "I2CWrapper.h"

// Default I2C address — SA0 pin low (0x1C) as wired on jonisk PCB
#define MMA8451_I2C_ADDRESS 0x1C

struct AccelData {
  float x; // g
  float y; // g
  float z; // g
};

class MMA8451 {
public:
  enum class Range : uint8_t {
    G2 = 0x00,
    G4 = 0x01,
    G8 = 0x02,
  };

  MMA8451(uint8_t addr = MMA8451_I2C_ADDRESS);

  esp_err_t begin(Range range = Range::G2);
  esp_err_t read(AccelData &out);

  AccelData data = {0.f, 0.f, 0.f};

private:
  uint8_t _addr;
  i2c_master_dev_handle_t _dev_handle = NULL;
  float _scale = 1.f / 4096.f; // default ±2g

  esp_err_t standby();
  esp_err_t active();
};

#endif // MMA8451_H
