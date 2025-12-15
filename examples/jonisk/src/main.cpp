#include "globals.h"
#include "objects.h"
#include "sdkconfig.h"

static const char *TAG = "Jonisk";

extern "C" void app_main(void) {
  blink.init();
  xTaskCreate(blink.updateTask, "blink", 2048, &blink, 0, NULL);
  while (1) {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
