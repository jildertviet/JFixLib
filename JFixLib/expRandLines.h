#ifndef EXP_RAND_LINES_H
#define EXP_RAND_LINES_H

#include "Event.h"

class expRandLines : public Event {
public:
  expRandLines(uint8_t x, uint8_t y, uint16_t width, uint16_t height, 
               uint8_t r, uint8_t g, uint8_t b, int16_t lifeTime, 
               uint16_t delayTime, uint8_t sizeRange);
  
  uint8_t sizeRange = 0;
  void update() override;
  void draw() override;
  void draw(floatColor **leds, int numLedsPerString, char numStrings,
            int horizontalPixelDistance) override;

  uint32_t lastTriggered = 0;
  uint16_t delayTime = 10;
  uint16_t w = 320;
  uint16_t h = 200;
};

#endif // EXP_RAND_LINES_H
