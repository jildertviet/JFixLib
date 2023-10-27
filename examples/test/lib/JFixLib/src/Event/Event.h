#ifndef JEVENT
#define JEVENT

#include "fabgl.h"
#include "JEnv.h"

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
  virtual void update(){checkLifeTime();};

  JEnv brightnessEnv;
  void triggerBrightnessEnv(unsigned short a, unsigned short s, unsigned short r, float b);

  float loc[2] = {0,0};
  unsigned char size[2] = {255, 255};
  float speed[2] = {1,0};
  // unsigned char color[3] = {255, 255, 255};
  RGBA8888 color = RGBA8888(255, 255, 255, 255);
  float brightness = 1.0;
  fabgl::Canvas* canvas = nullptr;
  boundariesMode limitMode = MODE_BOUNCE;
};
#endif // JEVENT
