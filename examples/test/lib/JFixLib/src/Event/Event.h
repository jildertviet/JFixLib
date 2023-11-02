#ifndef JEVENT
#define JEVENT

#include "fabgl.h"
#include "JEnv.h"
#include "FastLED.h"

enum boundariesMode{
  MODE_BOUNCE,
  MODE_RESET
};

class Event{
public:
  Event();
  ~Event();
  void testFunc();
  bool bActive = false;
  unsigned long endTime = 0;
  bool checkLifeTime();

  virtual void draw(){};
  virtual void draw(CRGB** leds, int numLedsPerString, char numStrings){};
  void (*writeRGB)(int, float, float, float, char, CRGB**) = nullptr;

  virtual void update(){checkLifeTime();};

  JEnv brightnessEnv;
  void triggerBrightnessEnv(unsigned short a, unsigned short s, unsigned short r, float b);

  float loc[2] = {0,0};
  float size[2] = {255, 255};
  float speed[2] = {1,0};
  // unsigned char color[3] = {255, 255, 255};
  // RGBA8888 color = RGBA8888(255, 255, 255, 255);
  float rgba[4] = {1,1,1,1};
  float brightness = 1.0;
  fabgl::Canvas* canvas = nullptr;
  boundariesMode limitMode = MODE_BOUNCE;
  int numLedsPerString = 1;
  int numLeds = 1;
};
#endif // JEVENT
