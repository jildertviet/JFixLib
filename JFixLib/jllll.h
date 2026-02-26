#pragma once
#include "jfixture_addr.h"
#include "ethernet_handler.h"
#include "motor_controller.h"

struct JllllSettings {
    uint8_t numLedsPerString = 234;
    uint8_t numStrings = 1;
    const uint8_t *pins = nullptr;
    gpio_num_t motorStepPin = GPIO_NUM_NC;
    gpio_num_t motorDirPin  = GPIO_NUM_NC;
    gpio_num_t motorEnPin   = GPIO_NUM_NC;
};

class Jllll : public jFixtureAddr {
public:
    void setup(const JllllSettings &s, uint8_t device_id) {
        if (s.pins)
            jFixtureAddr::setup(3, s.pins, s.numLedsPerString, J_WS2816B, s.numStrings);
        if (s.motorStepPin != GPIO_NUM_NC)
            motorController.init(s.motorStepPin, s.motorDirPin, s.motorEnPin);
        EthernetHandler::getInstance().init(device_id);
    }
    void update() override { jFixtureAddr::update(); }
};
