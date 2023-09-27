#pragma once
#include "JFixture.h"
#include "JEspnowDevice.h"

class JFixtureDimmer: public JEspnowDevice{
  public: 

  enum PwmMode{
    PWM12BIT
  };
  PwmMode pwmMode = PwmMode::PWM12BIT;
  char numChannels = 4;
  uint8_t* values;
  uint8_t* pins;
  float brightnessCurve[256];

  JFixtureDimmer(char numChannels = 4, uint8_t* pins = nullptr){
    values = new uint8_t[numChannels];
    if(pins){
      memcpy(pins, this->pins, numChannels);
      for(int i=0; i<numChannels; i++){
        ledcAttachPin(pins[i], 1+i);      
        switch(pwmMode){
          case PWM12BIT:
            ledcSetup(i+1, 9000, 12); // Hz / bitdepth
            ledcWrite(i+1, 0); // Turn all off
            break;
        }
      }
    }
    initCurve();
  }

  void initCurve(){ // 0.0 -- 1.0
    for(int i=0; i<256; i++){
      if(i < 30){
        float v = i / 30.0;
        v = pow(v, 0.5);
        v *= 30.0;
        brightnessCurve[i] = pow((v / 256.), 2.0);
      } else{
        brightnessCurve[i] = pow((i / 256.), 2.0);
      }
    }
  }

  void setLED(char channel, int value){ // Receives 0 - 255
    float v = brightnessCurve[value];
    values[channel] = value; // Save the goal-value
    if(pwmMode == PWM12BIT){
      value = v * 4096;
    } else{
      value = v * 256;
    }
    ledcWrite(channel + 1, value);
  }
 
  void showSucces() override{

  }
};
