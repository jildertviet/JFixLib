#pragma once
#include "NVSStorage.h"
#include "OTAUpdater.h"
#include "lagger.h"

class jFixture {
public:
  jFixture();
  virtual void init();
  void connectWiFi();
  virtual void update();

  void setBrightness(float b);
  float getBrightness();

  static jFixture *instance;

protected:
  float brightness = 1.0f;
  static const int NUM_LAGGERS = 4;
  Lagger laggers[NUM_LAGGERS];
  Lagger *brightnessLag;

  void updateLaggers();
};
