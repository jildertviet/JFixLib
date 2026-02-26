#pragma once
#include "Event.h"
#include "JWavetable.h"

enum BlendMode { BLEND_NONE, BLEND_MAX, BLEND_TEST };

class JOsc : public Event {
public:
    JOsc(JWavetable *w);
    JWavetable *w;
    void setWavetable(JWavetable *w);

    float phase = 0;
    float range = 1;
    float offset = 0;
    float frequency = 0.1f;

    void start();
    uint32_t startTime = 0;

    void draw(floatColor **leds, int numLedsPerString, char numStrings,
              int horizontalPixelDistance) override;
    void update() override;
    void setVal(char type, float value) override;

    uint32_t lastUpdated = 0;
};
