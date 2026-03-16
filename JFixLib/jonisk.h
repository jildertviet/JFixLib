#ifndef JFIX_EMULATION
#include "BQ25792.h"
#include "I2CWrapper.h"
#include "MMA8451.h"
#include "blink.h"
#include "jfixture.h"

class Jonisk : public jFixture {
public:
  Jonisk();
  void init() override;
  void update() override;
  static void updateTask(void *pvParameters);

private:
  BQ25792 *charger;
  MMA8451 accel;
};
#endif // !JFIX_EMULATION
