#include "JFixture.h"
// #include "JEspnowDevice.h"
#include "JFixtureDimmer.h"

#define WS2812B_STATUS_LED

class JJoniskSettings{
  public:
    JJoniskSettings(){};
    String networkName = "JV_";
    char numChannels = 4;
    uint8_t* pins = nullptr;
    char ledBuiltin = 5;
};

class JJonisk: public JFixtureDimmer{
  public:
    // void setup(String networkName, char numChannels = 4, uint8_t* pins = nullptr){
    //   JEspnowDevice::setup(networkName);
    //   JFixtureDimmer::setup(numChannels, pins);
    // }
    void setup(JJoniskSettings settings){
      JEspnowDevice::setup(settings.networkName);
      if(settings.pins){
        JFixtureDimmer::setup(settings.numChannels, settings.pins);
      }
      setLedBuiltin(settings.ledBuiltin);
    }
    void update() override{
      JFixtureDimmer::update();
    }
};
