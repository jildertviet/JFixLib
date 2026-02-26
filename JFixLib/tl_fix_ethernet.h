#pragma once
#include "jfixture_addr.h"
#include "ethernet_handler.h"
#include "tl_fix.h"  // reuse JTlFixSettings

class JTlFixEthernet : public jFixtureAddr {
public:
    void setup(const JTlFixSettings &s, uint8_t device_id) {
        if (s.pins)
            jFixtureAddr::setup(6, s.pins, s.numLedsPerString, J_WS2816B, s.numStrings);
        horizontalPixelDistance = s.horizontalPixelDistance;
        EthernetHandler::getInstance().init(device_id);
    }
    void update() override { jFixtureAddr::update(); }
};
