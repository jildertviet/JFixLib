#ifndef JEVENT_H
#define JEVENT_H

#include "JEnv.h"
#include "defines.h"
#include <string>
#include <vector>

#define MAX_ENV 6
#define NUM_CUSTOM_ARGS 12
#define NUM_PARAMETER_CONFIGS 8

enum boundariesMode { MODE_BOUNCE, MODE_RESET };

class ParameterConfig {
public:
  ParameterConfig();
  void init(float *b, float *ptr, char name);
  void update();
  void setFromBus();

  char name = '1'; // xywhrgBb
  float *busses = nullptr;
  short id = 0;
  float *ptr = nullptr;
  bool bActive = false;
};

class Event {
public:
  Event();
  virtual ~Event();

  ParameterConfig parameterConfigs[NUM_PARAMETER_CONFIGS];
  void linkBus(char name, char busIndex, float *busses);
  void initParameterConfigs();
  
  bool bActive = false;
  float bInvertHeight = 0.0f;
  uint32_t endTime = 0;
  bool checkLifeTime();
  void updateEnvelopes();
  uint32_t syncTime = 0;

  float viewport[2] = {0, 0};
  float viewportOffset[2] = {0, 0};
  int horizontalSpacing = 0;

  virtual void draw() {};
  virtual void draw(floatColor **leds, int numLedsPerString, char numStrings,
                    int horizontalPixelDistance) {};
  
  void (*writeRGB)(int, float, float, float, uint8_t, floatColor **) = nullptr;

  virtual void update();

  JEnv brightnessEnv;
  JEnv *envelopes[MAX_ENV] = {nullptr};
  void triggerBrightnessEnv(uint16_t a, uint16_t s, uint16_t r, float b);

  float loc[2] = {0, 0};
  float size[2] = {255, 255};
  float speed[2] = {1, 0};
  float rgba[4] = {1, 1, 1, 1};
  float brightness = 1.0f;
  boundariesMode limitMode = MODE_BOUNCE;
  int numLedsPerString = 1;
  int numLeds = 1;

  JEnv *getIdleEnv();
  void addEnv(char varName, float *dest, uint16_t a, uint16_t s,
              uint16_t r, float b, float bias, bool bKill = false);
  virtual void setVal(char type, float val);
  void setBusses(float *b, int num);
  bool bWaitForEnv = false;
  int id = -1;
  float *customBusses[NUM_CUSTOM_ARGS];
  void setCustomArg(uint8_t id, float val);

protected:
  uint32_t getMillis();
};

#endif // JEVENT_H
