#include "JOsc.h"
#include <cmath>

JOsc::JOsc(JWavetable *w) { this->w = w; }

void JOsc::update() {
    uint32_t now = getMillis() - syncTime;
    int deltaTime = (int)(now - lastUpdated);
    phase += (deltaTime / 1000.f) * frequency;
    phase = fmodf(phase, 1.0f);
    if (phase < 0)
        phase = 1.0f - phase;
    updateEnvelopes();
    checkLifeTime();
    lastUpdated = now;
}

void JOsc::start() {
    bActive = true;
    startTime = getMillis() - syncTime;
}

void JOsc::draw(floatColor **leds, int numLedsPerString, char numStrings,
                int horizontalPixelSpacing) {
    if (bWaitForEnv || !writeRGB)
        return;

    float x = loc[0] * viewport[0] - (viewportOffset[0] * viewport[0]);
    float y = loc[1] * viewport[1] - (viewportOffset[1] * viewport[1]);
    float width = size[0] * viewport[0];
    float yEnd = y + size[1] * viewport[1];

    int xReadPositions[2] = {0, horizontalPixelSpacing};
    int hPixels = (int)(numLedsPerString * size[1]);
    int yStart = (int)(numLedsPerString * loc[1]);

    for (int j = 0; j < numStrings; j++) {
        float xEnd = x + width;
        if (x > xReadPositions[j] || xEnd < xReadPositions[j] || yEnd < 0 ||
            x > viewport[0] || y > viewport[1])
            continue;

        if (x <= xReadPositions[j] && xEnd >= xReadPositions[j]) {
            float yDraw = y;
            if (yDraw < 0) yDraw = 0;
            float yDrawEnd = yEnd > numLedsPerString ? numLedsPerString : yEnd;
            (void)yDraw;
            (void)yDrawEnd;
            for (int i = 0; i < hPixels; i++) {
                if (yStart + i >= numLedsPerString)
                    continue;
                float pct = i / (float)hPixels;
                pct = fmodf(pct * range + offset + phase, 1.0f);
                float v = w->getValue(pct);
                writeRGB(yStart + i, rgba[0] * v * brightness,
                         rgba[1] * v * brightness, rgba[2] * v * brightness,
                         j, leds);
            }
        }
    }
}

void JOsc::setWavetable(JWavetable *w) { this->w = w; }

void JOsc::setVal(char type, float value) {
    Event::setVal(type, value);
    switch (type) {
    case 'f': frequency = value; break;
    case 'R': range = value; break;   // 'r' is reserved for red in Event::setVal
    case 'o': offset = value; break;
    case 'q': w->fillSineNorm(value); break;
    }
}
