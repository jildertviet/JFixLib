#include "led_beam.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// 1m JLedBeam: 60 WS2812B LEDs on GPIO 4, gate on GPIO 16
static JLedBeam beam;

extern "C" void app_main(void) {
    beam.init();

    JLedBeamSettings s;
    s.numLedsPerString = 60;
    s.pins = new uint8_t[1]{4};
    beam.setup(s);

    while (1) {
        beam.update();
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}
