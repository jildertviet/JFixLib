#ifndef JEVENT
#define JEVENT

#include "FastLED.h"
#include "JEnv.h"
#include "defines.h"

#define MAX_ENV 6
#define NUM_CUSTOM_ARGS 12

enum boundariesMode { MODE_BOUNCE, MODE_RESET };

class Event {
public:
  Event();
  ~Event();
  void testFunc();
  bool bActive = false;
  float bInvertHeight = false;
  unsigned long endTime = 0;
  bool checkLifeTime();
  void updateEnvelopes();
  unsigned long syncTime = 0;

  float *viewport;
  float *viewportOffset;
  int horizontalSpacing = 0;

  virtual void draw(){};
  virtual void draw(floatColor **leds, int numLedsPerString, char numStrings,
                    int horizontalPixelDistance){};
  void (*writeRGB)(int, float, float, float, char, floatColor **) = nullptr;

  virtual void update() { checkLifeTime(); };

  JEnv brightnessEnv;
  JEnv *envelopes[MAX_ENV] = {nullptr};
  void triggerBrightnessEnv(unsigned short a, unsigned short s,
                            unsigned short r, float b);

  float loc[2] = {0, 0};
  float size[2] = {255, 255};
  float speed[2] = {1, 0};
  // unsigned char color[3] = {255, 255, 255};
  // RGBA8888 color = RGBA8888(255, 255, 255, 255);
  float rgba[4] = {1, 1, 1, 1};
  float brightness = 1.0;
  boundariesMode limitMode = MODE_BOUNCE;
  int numLedsPerString = 1;
  int numLeds = 1;
  JEnv *getIdleEnv();
  void addEnv(char varName, float *dest, unsigned short a, unsigned short s,
              unsigned short r, float b, float bias, bool bKill = false);
  virtual void setVal(char type, float val);
  void setBusses(float *busses, int num);
  bool bWaitForEnv = false;
  int id = -1;
  float *busses[NUM_CUSTOM_ARGS];
  void setCustomArg(char id, float val);
};
#endif // JEVENT
