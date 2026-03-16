#include "expRandLines.h"
#include "jfix_platform.h"
#ifndef JFIX_EMULATION
#include "esp_random.h"
#endif
#include <cmath>

static uint32_t get_random(uint32_t min, uint32_t max) {
    if (min >= max) return min;
    return min + (esp_random() % (max - min));
}

expRandLines::expRandLines(uint8_t x, uint8_t y, uint16_t width, uint16_t height, 
                           uint8_t r, uint8_t g, uint8_t b, int16_t lifeTime, 
                           uint16_t delayTime, uint8_t sizeRange) {
    loc[0] = x / 255.0f;
    loc[1] = y / 255.0f;
    size[0] = (float)width;
    size[1] = (float)height;
    rgba[0] = r / 255.0f;
    rgba[1] = g / 255.0f;
    rgba[2] = b / 255.0f;
    
    if (lifeTime > 0) {
        endTime = getMillis() + lifeTime;
    }
    this->delayTime = delayTime;
    this->sizeRange = sizeRange;
}

void expRandLines::update() {
    Event::update();
    
    if (getMillis() > lastTriggered + delayTime) {
        lastTriggered = getMillis();
        
        if (get_random(0, 2) >= 1) {
            size[0] = (float)w;
            size[1] = std::pow(get_random(0, 1001) / 1000.0f, 4.0f) * sizeRange + 1.0f;
            loc[0] = 0;
            loc[1] = 0.5f + (std::pow(get_random(0, 1001) / 1000.0f, 4.0f) * 0.5f) * (get_random(0, 2) * 2 - 1);
        } else {
            size[0] = std::pow(get_random(0, 1001) / 1000.0f, 4.0f) * sizeRange + 1.0f;
            size[1] = (float)h;
            loc[0] = 0.5f + (std::pow(get_random(0, 1001) / 1000.0f, 4.0f) * 0.5f) * (get_random(0, 2) * 2 - 1);
            loc[1] = 0;
        }
    }
}

void expRandLines::draw() {
    // Legacy draw call for canvas-based rendering
}

void expRandLines::draw(floatColor **leds, int numLedsPerString, char numStrings,
                        int horizontalPixelDistance) {
    // Implementation for addressable LEDs can be added here if needed,
    // similar to JRect but with randomized behavior.
}
