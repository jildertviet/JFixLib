#pragma once

// #define WS2812B_STATUS_LED

#ifdef  WS2812B_STATUS_LED
#include "FastLED.h"
#define WS2812B_PIN 25 // Have to hardcode this here...

CRGB statusLed[1];

class JWS2812B_status{
  public:
  JWS2812B_status(){
    FastLED.addLeds<WS2812B, WS2812B_PIN, GRB>(statusLed, 1);
  }
  static void digitalWriteBuiltinLedAddr(char pin, char status){
    if(status){
      statusLed[0] = CRGB(0, 0, 40); 
    } else{
      statusLed[0] = CRGB::Black; 
    }
    FastLED.show();
  }
};
#else
class JWS2812B_status{
  public: 
    JWS2812B_status(){};
};

#endif

