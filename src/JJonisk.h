#include "JFixture.h"

class JJonisk: public JFixture{
  public:
    void setup(string networkName){
      JFixture::setup(networkName);    
    }
    void update() override{
      JFixture::update(); 
    }
}
