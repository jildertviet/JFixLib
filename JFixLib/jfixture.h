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

  void setId(int newId);
  int getId() const { return id; }

  static jFixture *instance;

protected:
  int id = -1;
  float brightness = 1.0f;
  float viewport[2] = {20.0f, 144.0f};
  float viewportOffset[2] = {0.0f, 0.0f};
  bool bStatic = false;
  static const int NUM_LAGGERS = 4;
  Lagger laggers[NUM_LAGGERS];
  Lagger *brightnessLag;

  void updateLaggers();
};
