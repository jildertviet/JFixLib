#include "JFixture.h"
// #include "JEspnowDevice.h"
// #include "JFixtureDimmer.h"
#include "JEthernetDevice.h"
#include "JFixtureAddr.h"

class JllllSettings {
public:
  JllllSettings(){};
  String networkName = "JV_";
  char numChannels = 3;
  uint8_t numLedsPerString = 144;
  char numStrings = 1;
  char ledBuiltin = 5;
  uint8_t *pins = nullptr;
};

class Jllll : public JFixtureAddr, public JEthernetDevice {
public:
  // void setup(String networkName, char numChannels = 4, uint8_t* pins =
  // nullptr){
  //   JEspnowDevice::setup(networkName);
  //   JFixtureDimmer::setup(numChannels, pins);
  // }

  void setup(JllllSettings settings) {
    bEspnowEnabled = false;
    JEspnowDevice::setup(settings.networkName);

    if (initEthernet(id, myAddr)) {
      Serial.println("Ethernet is OK");
    }
    if (settings.pins) {
      JFixtureAddr::setup(settings.numChannels, settings.pins,
                          settings.numLedsPerString,
                          JAddressableMode::J_WS2816B, settings.numStrings);
    }
    setLedBuiltin(settings.ledBuiltin);
    // horizontalPixelDistance = settings.horizontalPixelDistance;
    sendPing(true);
  }

  void update() override {
    JFixtureAddr::update();
    JEthernetDevice::receiveUDP(receive);

    // }
    // JFixture::update();
  }
  void blink(char num = 1, short dur = 100, short delayTime = 100,
             char channel = 0) override {}
};
