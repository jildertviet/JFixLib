#include "JFixture.h"
// #include "JEspnowDevice.h"
// #include "JFixtureDimmer.h"
#include "JEthernetDevice.h"
#include "JFixtureAddr.h"
// #include "JTlFix.h"

// class JTlFixSettings {
// public:
//   JTlFixSettings(){};
//   String networkName = "JV_";
//   char numChannels = 6;
//   uint8_t numLedsPerString = 144;
//   char numStrings = 2;
//   char ledBuiltin = 5;
//   uint8_t *pins = nullptr;
//   short horizontalPixelDistance = 10;
// };

class JTlFixEthernet : public JFixtureAddr, public JEthernetDevice {
public:
  // void setup(String networkName, char numChannels = 4, uint8_t* pins =
  // nullptr){
  //   JEspnowDevice::setup(networkName);
  //   JFixtureDimmer::setup(numChannels, pins);
  // }
  void setup(JTlFixSettings settings) {
    JEspnowDevice::setup(settings.networkName);
    if (settings.pins) {
      JFixtureAddr::setup(settings.numChannels, settings.pins,
                          settings.numLedsPerString,
                          JAddressableMode::J_WS2816B, settings.numStrings);
    }
    delay(1000);
    if (initEthernet(id, myAddr)) {
      Serial.println("Ethernet is OK");
    }

    setLedBuiltin(settings.ledBuiltin);
    horizontalPixelDistance = settings.horizontalPixelDistance;
    bEspnowPingEnabled = false;
    // sendPing(true);
  }

  void update() override {
    JFixtureAddr::update();
    JEthernetDevice::receiveUDP(receive);
    // JFixture::update();
  }
  void blink(char num = 1, short dur = 100, short delayTime = 100,
             char channel = 0) override {}
};
