#include "tl_fix.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// JTlFix: 144 WS2816B LEDs, 2 strings on GPIO 22 and 23
static JTlFix fix;

extern "C" void app_main(void) {
    fix.init();

    JTlFixSettings s;
    s.pins = new uint8_t[2]{22, 23};
    fix.setup(s);

    while (1) {
        fix.update();
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}
