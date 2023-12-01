// #include <WiFi.h>
#define WS2812B_STATUS_LED
// #define J_DATA_PINS  (int[]){3}
#include "JFixLib.h"

// JJonisk jonisk;
JTlFix jTl;

void setup(){
  // JJoniskSettings settings;
  // settings.networkName = "JV_";
  // settings.pins = new uint8_t[4]{12,13,22,23};
  // settings.numChannels = 4;
  // settings.ledBuiltin = 5;
  // jonisk.setup(settings);
  JTlFixSettings settings;
  settings.networkName = "JV_";
  settings.pins = new uint8_t[2]{22, 23}; // Test on pin3
  jTl.setup(settings);
  // jonisk.writeStatusLedPtr = &jonisk.digitalWriteBuiltinLedAddr;
}

void loop(){
  // jonisk.update();
  jTl.update();
}
