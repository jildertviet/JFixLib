//#include <ESP32Lib.h>
#include "Event.h"

class JRect: public Event{
public:
  // JRect(unsigned char x, unsigned char y, unsigned short w, unsigned short h, unsigned char r, unsigned char g, unsigned char b, short lifeTime, float speedX, float speedY);
  JRect(unsigned short lifeTime = 0);
  // void draw() override;
  void draw(CRGB** leds, int numLedsPerString, char numStrings) override;
  void update() override;
};
