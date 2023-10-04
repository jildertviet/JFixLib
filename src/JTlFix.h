#include "JFixture.h"
// #include "JEspnowDevice.h"
// #include "JFixtureDimmer.h"
#include "JFixtureAddr.h"

class JTlFixSettings{
  public:
    JTlFixSettings(){};
    String networkName = "JV_";
    char numChannels = 6;
    char ledBuiltin = 5;
    uint8_t* pins = nullptr;
};

class JTlFix: public JFixtureAddr{
  public:
    // void setup(String networkName, char numChannels = 4, uint8_t* pins = nullptr){
    //   JEspnowDevice::setup(networkName);
    //   JFixtureDimmer::setup(numChannels, pins);
    // }
    void setup(JJoniskSettings settings){
      JEspnowDevice::setup(settings.networkName);
      if(settings.pins){
        JFixtureAddr::setup(settings.numChannels, settings.pins);
      }
      setLedBuiltin(settings.ledBuiltin);
    }
    void update() override{
      JFixture::update(); 
    }
    void blink(char num=1, short dur=100, short delayTime=100, char channel=0) override{
    }
};
