#include "JFixture.h"
// #include "JEspnowDevice.h"
// #include "JFixtureDimmer.h"
// #define
#include "JFixtureAddr.h"

class JLedBeamSettings {
public:
  JLedBeamSettings(){};
  String networkName = "JV_";
  char numChannels = 3;
  uint8_t numLedsPerString = ADAFRUIT_NUM_PIXELS;
  char numStrings = 1;
  char ledBuiltin = 5;
  uint8_t *pins = nullptr;
  short horizontalPixelDistance = 10;
};

class JLedBeam : public JFixtureAddr {
public:
  // void setup(String networkName, char numChannels = 4, uint8_t* pins =
  // nullptr){
  //   JEspnowDevice::setup(networkName);
  //   JFixtureDimmer::setup(numChannels, pins);
  // }
  void setup(JLedBeamSettings settings) {
    JEspnowDevice::setup(settings.networkName);
    if (settings.pins) {
      JFixtureAddr::setup(settings.numChannels, settings.pins,
                          settings.numLedsPerString,
                          JAddressableMode::J_WS2812B, settings.numStrings);
    }
    setLedBuiltin(settings.ledBuiltin);
    horizontalPixelDistance = settings.horizontalPixelDistance;
    sendPing(true);
    pinMode(16, OUTPUT); // PWM signal or simply use it as a gate
    digitalWrite(16, HIGH);
  }

  void update() override {
    JFixtureAddr::update();
    // JFixture::update();
  }
  void blink(char num = 1, short dur = 100, short delayTime = 100,
             char channel = 0) override {}
};
