#if ENABLE_ETHERNET
#if ENABLE_MOTOR
#include "JFixture.h"
// #include "JEspnowDevice.h"
// #include "JFixtureDimmer.h"
#include "JEthernetDevice.h"
#include "JFixtureAddr.h"
#include "JMotorController.h"

class JllllSettings {
public:
  JllllSettings(){};
  String networkName = "JV_";
  char numChannels = 3;
  uint8_t numLedsPerString = 234;
  char numStrings = 1;
  char ledBuiltin = 5;
  uint8_t *pins = nullptr;
};

class Jllll : public JFixtureAddr,
              public JEthernetDevice,
              public JMotorController {
public:
  // void setup(String networkName, char numChannels = 4, uint8_t* pins =
  // nullptr){
  //   JEspnowDevice::setup(networkName);
  //   JFixtureDimmer::setup(numChannels, pins);
  // }

  void setup(JllllSettings settings) {
    // bEspnowEnabled = false;
    JEspnowDevice::setup(settings.networkName);
    receiveMotorCommandsPtr = &receiveMotorCommands;
    initMotor();

    delay(1000);
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

    // sendPing(true); // To set macaddr?
    bEspnowPingEnabled = false;
    WiFi.mode(WIFI_OFF);
  }

  void update() override {
    JFixtureAddr::update();
    JEthernetDevice::receiveUDP(receive);
    // JMotorController::updateMotor(); // Happens in other core... ?

    // }
    // JFixture::update();
  }
  void blink(uint8_t num = 1, short dur = 100, short delayTime = 100,
             uint8_t channel = 0) override {}
};

#endif // ethernet
#endif // motor
