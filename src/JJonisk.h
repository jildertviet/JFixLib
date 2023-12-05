#include "JFixture.h"
// #include "JEspnowDevice.h"
#include "JFixtureDimmer.h"

class JJoniskSettings {
public:
  JJoniskSettings(){};
  String networkName = "JV_";
  char numChannels = 4;
  uint8_t *pins = nullptr;
  char ledBuiltin = 5;
  char batterpyPin = 34;
};

class JJonisk : public JFixtureDimmer {
public:
  // void setup(String networkName, char numChannels = 4, uint8_t* pins =
  // nullptr){
  //   JEspnowDevice::setup(networkName);
  //   JFixtureDimmer::setup(numChannels, pins);
  // }
  void setup(JJoniskSettings settings) {
    JEspnowDevice::setup(settings.networkName);
    if (settings.pins) {
      JFixtureDimmer::setup(settings.numChannels, settings.pins);
    }
    setLedBuiltin(settings.ledBuiltin);
    this->batteryPin = settings.batterpyPin;
    if (batteryPin > 0) {
      pinMode(batteryPin, INPUT);
    }
    sendPing(true); // Send ping @ boot
  }
  void update() override { JFixtureDimmer::update(); }

  int measureBattery() override {
    if (batteryPin > 0) {
      int numSamples = 5;
      int total = 0; // To sum the read values
      for (int i = 0; i < numSamples; i++) {
        total += analogRead(batteryPin);
        delay(20);
      }
      total /= numSamples;
      return total; // 0 <> 4095
    }
    return 0;
  }
};
