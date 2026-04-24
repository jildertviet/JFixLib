#pragma once
#include "NVSStorage.h"
#include "jfix_platform.h"
#include "lagger.h"

#ifdef JFIX_ENABLE_OTA
#include "OTAUpdater.h"
#endif

#ifdef JFIX_ENABLE_GRAPHICS
class jFixtureGraphics; // forward declaration for asGraphics()
#endif

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
  bool macMatches(const uint8_t *other, size_t len) const;

  // Set lag time for lagger at index (0 = brightness lagger)
  void setLagTime(int lagger_id, float lag_ms);

  // Set the background RGBA floor (pixels are clamped to at least r,g,b when
  // rendered).
  void setBackground(float r, float g, float b, float a);

  // Set the viewport X/Y offset applied to all events.
  void setViewportOffset(float x, float y);

#ifdef JFIX_ENABLE_GRAPHICS
  // Returns this as jFixtureGraphics* if the instance is one, nullptr
  // otherwise. Avoids dynamic_cast (RTTI is disabled in ESP-IDF builds).
  virtual jFixtureGraphics *asGraphics() { return nullptr; }
#endif

  // Parameter buses: shared float values that events can subscribe to via
  // linkBus.
  static const int NUM_PARAMETER_BUSSES = 16;
  float parameterBusses[NUM_PARAMETER_BUSSES] = {};

  void setParameterBus(int index, float value);
  void setParameterBusN(int startIndex, const float *values, int count);

  static jFixture *instance;

  float rgbaBackground[4] = {0.0f, 0.0f, 0.0f, 0.0f};

  // Set by JTimeScheduler::tick() when an IDLE or LIGHT_SLEEP slot is active.
  // Consumed by jFixtureAddr::update() → blanks pixels.
  bool bIdle = false;

  // If non-zero, jFixtureAddr::update() will enter esp_light_sleep after
  // blanking pixels. Set (and consumed) by the scheduler each tick.
  uint64_t nextLightSleepUs = 0;

protected:
  float viewport[2] = {20.0f, 120.0f};
  int id = -1;
  uint8_t baseMac[6] = {};
  uint8_t staMac[6] = {};
  float brightness = 1.0f;
  float viewportOffset[2] = {0.0f, 0.0f};
  bool bStatic = false;
  static const int NUM_LAGGERS = 4;
  Lagger laggers[NUM_LAGGERS];
  Lagger *brightnessLag;

  void updateLaggers();
};
