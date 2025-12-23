#include "BQ25792.h"
#include "I2CWrapper.h"
#include "blink.h"
#include "jfixture.h"

class Jonisk : public jFixture {
public:
  Jonisk();
  void init();

private:
  BQ25792 *charger;
};
