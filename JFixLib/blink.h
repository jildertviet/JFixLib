#pragma once
#include "globals.h"
#include "hal/gpio_types.h"

class Blink {
public:
  Blink();
  static void updateTask(void *pvParameters);
  void update();
  void init();
  void setInterval(uint16_t on_ms, uint16_t off_ms);

private:
  bool bLedState = 0;
  uint16_t blinkTimes[2] = {900, 100};
  static const gpio_num_t pin = BLINK_GPIO;
};

extern Blink blink;
