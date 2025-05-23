#pragma once

#include "Event.h"
#include "ofNoise.h"

class JEvent_Perlin : public Event {
public:
  JEvent_Perlin() {
    busses[0] = &noiseScale;
    busses[1] = &noiseTimeScale;
    busses[2] = &horizontalPixelOffset;
  };
  // JFixtureAddr* parent = nullptr;
  float noiseScale = 0.01;
  float noiseTimeScale = 0.0005;
  float horizontalPixelOffset = 0;
  // int horizontalPixelDistance = 10; // Beams are 10 pixels from eachother

  void update() {
    Event::update(); // check parameterMappers
    updateEnvelopes();
    checkLifeTime();
  }

  void draw(floatColor **leds, int numLedsPerString, char numStrings,
            int horizontalPixelDistance) override {
    if (leds && brightness) {
      float y =
          loc[1] * viewport[1] - (viewportOffset[1] * viewport[1]); // In pixels
      float h = size[1] * viewport[1];                              // in Pixels
      float yEnd = y + h;

      unsigned long t = millis() - syncTime;
      for (float j = 0; j < numStrings; j++) {
        if (yEnd < 0 || y > viewport[1])
          continue;
        if (y < 0) // Clip in range
          y = 0;
        if (yEnd > numLedsPerString)
          yEnd = numLedsPerString;

        for (float i = y; i < yEnd; i++) {
          float val =
              ofNoise(i * noiseScale,
                      ((j * horizontalPixelDistance) + horizontalPixelOffset) *
                          noiseScale,
                      t * noiseTimeScale);
          // val = pow(
          // val, 2.0); // Already @ writeRGB. Edit; do it anyway, looks
          // better
          val = val * val;
          // if (val < 0.005) // Do this with a background
          // val = 0.005;
          if (writeRGB)
            writeRGB(i, val * brightness * rgba[0], val * brightness * rgba[1],
                     val * brightness * rgba[2], j, leds);
        }
      }
    }
  }
};
