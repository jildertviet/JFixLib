#ifndef JEVENT_PERLIN_H
#define JEVENT_PERLIN_H

#include "Event.h"

class JEvent_Perlin : public Event {
public:
  JEvent_Perlin();
  
  float noiseScale = 0.01f;
  float noiseTimeScale = 0.0005f;
  float horizontalPixelOffset = 0.0f;

  float zPhase = 0.0f;
  float yPhase = 0.0f;
  uint32_t lastMs = 0;
  uint32_t lastLogMs = 0;
  uint32_t maxDtSeen = 0;

  void update() override;
  void draw(floatColor **leds, int numLedsPerString, char numStrings,
            int horizontalPixelDistance) override;
};

#endif // JEVENT_PERLIN_H
