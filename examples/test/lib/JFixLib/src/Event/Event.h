#ifndef JEVENT
#define JEVENT

#include "fabgl.h"
#include "JEnv.h"
#include "FastLED.h"

#define MAX_ENV 6

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
  void updateEnvelopes();

  float* viewport;
  float* viewportOffset;
  int horizontalSpacing = 0;
                                
  virtual void draw(){};
  virtual void draw(CRGB** leds, int numLedsPerString, char numStrings, int horizontalPixelDistance){};
  void (*writeRGB)(int, float, float, float, char, CRGB**) = nullptr;

  virtual void update(){checkLifeTime();};

  JEnv brightnessEnv;
  JEnv* envelopes[MAX_ENV] = {nullptr};
  void triggerBrightnessEnv(unsigned short a, unsigned short s, unsigned short r, float b);

  float loc[2] = {0,0};
  float size[2] = {255, 255};
  float speed[2] = {1,0};
  // unsigned char color[3] = {255, 255, 255};
  // RGBA8888 color = RGBA8888(255, 255, 255, 255);
  float rgba[4] = {1,1,1,1};
  float brightness = 1.0;
  // fabgl::Canvas* canvas = nullptr;
  boundariesMode limitMode = MODE_BOUNCE;
  int numLedsPerString = 1;
  int numLeds = 1;
  JEnv* getIdleEnv();
  void addEnv(char varName, float* dest, unsigned short a, unsigned short s, unsigned short r, float b, float bias, bool bKill=false);
  virtual void setVal(char type, float val);
  bool bWaitForEnv = false;
  int id = -1;
};
#endif // JEVENT
