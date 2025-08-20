#pragma once

#include "Event.h"

class JEvent_Perlin : public Event {
public:
  JEvent_Perlin();
  // JFixtureAddr* parent = nullptr;
  float noiseScale = 0.01;
  float noiseTimeScale = 0.0005;
  float horizontalPixelOffset = 0;
  // int horizontalPixelDistance = 10; // Beams are 10 pixels from eachother

  void update();
  void draw(floatColor **leds, int numLedsPerString, char numStrings,
            int horizontalPixelDistance) override;
};
