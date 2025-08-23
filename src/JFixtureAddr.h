#ifndef JFIXTURE_ADDR

// #define ADAFRUIT_NEOPIXEL // Set this is in the .ini file with build_flags =
// -D ADAFRUIT_NEOPIXEL=1
#ifdef ADAFRUIT_NEOPIXEL
#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel pixels(ADAFRUIT_NUM_PIXELS, ADAFRUIT_DATA_PIN,
                         NEO_GRB + NEO_KHZ800);
#elif NEOPIXELBUS
#include <NeoPixelBus.h>
#ifndef NEOPIXELBUS_PIN
#define NEOPIXELBUS_PIN 22
#endif
#ifndef NEOPIXELBUS_NUMLEDS
#define NEOPIXELBUS_NUMLEDS 234
#endif
#else
#define JFIX_FASTLED
#include "FastLED.h"
#include "FastLedContstants.h"
#endif

#include "JFixture.h"
#include "JFixtureGraphics.h"
#include "ofNoise.h"

class JFixtureAddr : public JFixtureGraphics {
public:
  floatColor **leds = nullptr;
  CRGB **ledsToWrite = nullptr; // Use this array for the brightness curve. The
                                // other is for reading back
  float brightnessCurve[256];
  int numLedsPerString = 1;
  char numStrings = 1;
  int horizontalPixelDistance = 10; // Beams are 10 pixels from eachother
  unsigned long lastUpdatedLive = 0;

  JFixtureAddr();
  enum JAddressableMode { J_WS2812B, J_WS2816B };
  enum JDrawMode { TEST, TEST_PERLIN, LIVE };
  JDrawMode drawMode = LIVE;
  // JDrawMode drawMode = TEST_PERLIN;
  JAddressableMode ledMode; // Type of LEDS

  void setup(char numColorChannels = 6, const uint8_t *pins = nullptr,
             short numLedsPerString = 60, JAddressableMode mode = J_WS2812B,
             int numStrings = 1);

  void initCurve();

  void setLED(char channel, int value) { // Receives 0 - 255
  }

  static void writeRGB(int id, float r, float g, float b, uint8_t channel,
                       floatColor **leds);

  void writeRGBHard(int id, float r, float g, float b, uint8_t channel,
                    floatColor **leds);

  void testLED();
  void showSucces() override {}
  void show() override{};
  void setChannel(uint8_t i, float v) override;
  void allBlack(bool bWrite = false);
  void writeLeds();
  void update() override;

  int ledIndex = 0;

  void blink(uint8_t num = 1, short dur = 100, short delayTime = 100,
             uint8_t channel = 0) override;
};

#define JFIXTURE_ADDR
#endif
