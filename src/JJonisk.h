#include "JFixture.h"
// #include "JEspnowDevice.h"
#include "JFixtureDimmer.h"

class JJonisk: public JFixtureDimmer{
  public:
    void setup(String networkName) override{
      JEspnowDevice::setup(networkName);    
    }
    void update() override{
      JFixture::update(); 
    }
    void blink(char num=1, short dur=100, short delayTime=100, char channel=0) override{
      for(int i=0; i<num; i++){
        setLED(0, 200);
        delay(dur);
        setLED(0, 0);
        delay(delayTime);
      }
    }
};
