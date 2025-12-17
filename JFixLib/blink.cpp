#include "blink.h"
#include "driver/gpio.h"

Blink blink;

Blink::Blink() {}

void Blink::init() {
  gpio_reset_pin(pin);
  gpio_set_direction(pin, GPIO_MODE_OUTPUT);
}

void Blink::updateTask(void *pvParameters) {
  Blink *blink = static_cast<Blink *>(pvParameters);
  while (1) {
    blink->update();
  }
}

void Blink::update() {
  gpio_set_level(pin, bLedState);
  vTaskDelay(blinkTimes[bLedState] / portTICK_PERIOD_MS);
  bLedState = !bLedState;
}
