// #include <WiFi.h>
#define WS2812B_STATUS_LED
#define J_DATA_PIN  3
#include "../../../src/JFixLib.h"

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
  settings.numChannels = 6;
  settings.ledBuiltin = 5;
  settings.pins = new uint8_t[3]; // Test on pin3
  // jonisk.writeStatusLedPtr = &jonisk.digitalWriteBuiltinLedAddr;
}

void loop(){
  // jonisk.update();
}
