#ifndef JRECT_H
#define JRECT_H

#include "Event.h"

class JRect : public Event {
public:
  JRect(uint16_t lifeTime = 0);
  
  void draw(floatColor **leds, int numLedsPerString, char numStrings,
            int horizontalPixelDistance) override;
  void update() override;
  void setVal(char type, float value) override;
};

#endif // JRECT_H
