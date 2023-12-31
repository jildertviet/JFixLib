#ifndef OSC_J
#define OSC_J

#include "Arduino.h"
#include "Event.h"
#include "JWavetable.h"

enum BlendMode { BLEND_NONE, BLEND_MAX, BLEND_TEST };

class JOsc : public Event {
public:
  JOsc(JWavetable *w);
  JWavetable *w;
  void setWavetable(JWavetable *w);
  // void update();
  // void write(float* data, int num, BlendMode=BLEND_NONE);
  float phase;
  float range = 1;
  float offset = 0;
  float frequency = 0.1;
  void start();
  double startTime = 0;
  bool bActive = false;

  void draw(floatColor **leds, int numLedsPerString, char numStrings,
            int horizontalPixelDistance) override;
  void update() override;
  void setVal(char type, float value) override;
  unsigned long lastUpdated = 0;
};

#endif
