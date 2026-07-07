#include "jonisk.h"

static const char *TAG = "Jonisk";

extern "C" void app_main(void) {
  Jonisk jonisk;
  jonisk.init();
  while (1) {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
