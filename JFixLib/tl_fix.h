#pragma once
#include "jfixture_addr.h"

struct JTlFixSettings {
    uint8_t numLedsPerString = 144;
    uint8_t numStrings = 2;
    const uint8_t *pins = nullptr;
    int horizontalPixelDistance = 10;
};

class JTlFix : public jFixtureAddr {
public:
    void setup(const JTlFixSettings &s) {
        if (s.pins)
            jFixtureAddr::setup(6, s.pins, s.numLedsPerString, J_WS2816B, s.numStrings);
        horizontalPixelDistance = s.horizontalPixelDistance;
    }
    void update() override { jFixtureAddr::update(); }
};
