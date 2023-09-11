#include "JFixture.h"

class JFixtureDimmer: public JFixture{
  public: 

  enum PwmMode{
    PWM12BIT
  };
  PwmMode pwmMode = PwmMode::PWM12BIT;
  char numChannels = 4;
  uint8_t* values;
  uint8_t* pins;

  JFixtureDimmer(char numChannels = 4, uint8_t* pins = nullptr){
    values = new uint8_t[numChannels];
    if(pins){
      memcpy(pins, this->pins, numChannels);
      for(int i=0; i<numChannels; i++){
        ledcAttachPin(pins[i], 1+i);      
        switch(pwmMode){
          case PWM12BIT:
            ledcSetup(1+i, 9000, 12); // Hz / bitdepth
            ledcWrite(i+1, 0); // Turn all off
            break;
        }
      }
    }
  }

  void showSucces() override{

  }
};
