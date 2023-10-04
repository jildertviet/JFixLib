#pragma once
#include "JFixture.h"
#include "JEspnowDevice.h"
#include "FastLED.h"

// Addressable leds 
class JFixtureAddr: public JEspnowDevice{
  public: 
  CRGB* leds = nullptr;
  float brightnessCurve[256];

  JFixtureAddr(){};

  void setup(char numChannels = 6, const uint8_t* pins = nullptr, short numLeds = 60){
    channels = new float[numChannels];
    leds = new CRGB[numLeds];
    FastLED.addLeds<WS2812B, J_DATA_PIN>(leds, numLeds);
    initCurve();
    testLED();
  }

  void initCurve(){ // 0.0 -- 1.0
    for(int i=0; i<256; i++){
      brightnessCurve[i] = pow((i / 256.), 2.0);
    }
  }

  void setLED(char channel, int value){ // Receives 0 - 255
  
  }

  void testLED(){

  };
  void showSucces() override{
  }
  void show() override{
  
  };

  void setChannel(char i, float v) override{
    for(int i=0; i<numChannels; i++){
      channels[i] = v;
    }
  }
};
