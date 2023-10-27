#include "Arduino.h"

#include "Event.h"

class expRandLines: public Event{
public:
    expRandLines(unsigned char x, unsigned char y, unsigned short width, unsigned short height, unsigned char r, unsigned char g, unsigned char b, short lifeTime, unsigned short delayTime, unsigned char sizeRange);
    unsigned char sizeRange = 0;
    void update() override;
    void draw() override;
    unsigned long lastTriggered = 0;
    unsigned short delayTime = 10;
    unsigned short w = 320;
    unsigned short h = 200;
};
