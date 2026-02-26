#include "tl_fix_ethernet.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// JTlFixEthernet: 144 WS2816B LEDs, 2 strings on GPIO 22 and 27
// UDP on port 1111, static IP: 192.168.1.{device_id}
static JTlFixEthernet fix;

extern "C" void app_main(void) {
    fix.init();

    JTlFixSettings s;
    s.pins = new uint8_t[2]{22, 27};
    fix.setup(s, 1); // device_id 1 → 192.168.1.1

    while (1) {
        fix.update();
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}
