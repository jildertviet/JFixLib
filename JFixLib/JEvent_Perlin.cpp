#include "JEvent_Perlin.h"
#include "ofNoise.h"
#include <math.h>

JEvent_Perlin::JEvent_Perlin() {
  customBusses[0] = &noiseScale;
  customBusses[1] = &noiseTimeScale;
  customBusses[2] = &horizontalPixelOffset;
}

void JEvent_Perlin::update() { Event::update(); }

void JEvent_Perlin::draw(floatColor **leds, int numLedsPerString,
                         char numStrings, int horizontalPixelDistance) {
  if (leds && brightness > 0) {
    float y = loc[1] * viewport[1] - (viewportOffset[1] * viewport[1]);
    float h = size[1] * viewport[1];
    float yEnd = y + h;

    uint32_t now = getMillis();
    if (lastMs != 0) {
      uint32_t dt = now - lastMs;
      if (dt > 40)
        dt = 40; // clamp stalls (WiFi wake, etc.) to ~2 frames
      zPhase += (float)dt * noiseTimeScale;
      // zPhase =
      // fmodf(zPhase, 256.0f); // keep within simplex period, avoid FP drift
    }
    lastMs = now;

    for (int j = 0; j < (int)numStrings; j++) {
      if (yEnd < 0 || y > viewport[1])
        continue;

      float currentY = (y < 0) ? 0 : y;
      float currentYEnd =
          (yEnd > numLedsPerString) ? (float)numLedsPerString : yEnd;

      for (float i = currentY; i < currentYEnd; i++) {
        float yCoord = ((j * horizontalPixelDistance) + horizontalPixelOffset) *
                           noiseScale +
                       zPhase * 0.37f;
        float val = ofNoise(i * noiseScale, yCoord, zPhase);

        val = val * val; // Square for better look
        // val = abs(val);

        if (writeRGB) {
          writeRGB((int)i, val * brightness * rgba[0],
                   val * brightness * rgba[1], val * brightness * rgba[2],
                   (uint8_t)j, leds);
        }
      }
    }
  }
}
