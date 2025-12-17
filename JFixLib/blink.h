#pragma once
#include "globals.h"
#include "hal/gpio_types.h"

class Blink {
public:
  Blink();
  static void updateTask(void *pvParameters);
  void update();
  void init();

private:
  bool bLedState = 0;
  const uint16_t blinkTimes[2] = {100, 900};
  static const gpio_num_t pin = BLINK_GPIO;
};

extern Blink blink;
