#include "jllll.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Jllll: 234 WS2816B LEDs on GPIO 22 + W5500 Ethernet + stepper motor
// Set motor pins to match your hardware before flashing
static Jllll jllll;

extern "C" void app_main(void) {
    jllll.init();

    JllllSettings s;
    s.pins = new uint8_t[1]{22};
    // s.motorStepPin = GPIO_NUM_32;
    // s.motorDirPin  = GPIO_NUM_33;
    // s.motorEnPin   = GPIO_NUM_26;
    jllll.setup(s, 1); // device_id 1 → 192.168.1.1

    while (1) {
        jllll.update();
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}
