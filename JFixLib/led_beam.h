#pragma once
#include "jfixture_addr.h"
#include "jfix_platform.h"
#ifndef JFIX_EMULATION
#include "driver/gpio.h"
#endif

struct JLedBeamSettings {
    uint8_t numLedsPerString = 60;
    const uint8_t *pins = nullptr;    // array of GPIO pins per string
    int horizontalPixelDistance = 10;
    gpio_num_t gatePin = GPIO_NUM_16; // PWM gate, driven HIGH on setup
};

class JLedBeam : public jFixtureAddr {
public:
    void setup(const JLedBeamSettings &s) {
        if (s.pins)
            jFixtureAddr::setup(3, s.pins, s.numLedsPerString, J_WS2812B, 1);
        horizontalPixelDistance = s.horizontalPixelDistance;
#ifndef JFIX_EMULATION
        gpio_set_direction(s.gatePin, GPIO_MODE_OUTPUT);
        gpio_set_level(s.gatePin, 1);
#endif
    }

    // Emulation: setup without pins (no hardware needed)
    void setupEmulation(uint16_t numLeds = 120) {
        jFixtureAddr::setup(3, nullptr, numLeds, J_WS2812B, 1);
    }

    void update() override { jFixtureAddr::update(); }
};
