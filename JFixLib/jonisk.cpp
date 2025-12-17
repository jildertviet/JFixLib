#include "jonisk.h"

Jonisk::Jonisk() {}

void Jonisk::init() {
  blink.init();
  xTaskCreate(blink.updateTask, "blink", 2048, &blink, 0, NULL);
}
