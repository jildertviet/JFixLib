// #include <WiFi.h>
#define WS2812B_STATUS_LED
// #define J_DATA_PINS  (int[]){3}
#include "JFixLib.h"

JJonisk jonisk;

void setup() {
  JJoniskSettings settings;
  settings.networkName = "JV_";
  settings.pins = new uint8_t[4]{16, 17, 18, 19};
  settings.numChannels = 4;
  settings.ledBuiltin = 5;
  jonisk.setup(settings);
  // jonisk.writeStatusLedPtr = &jonisk.digitalWriteBuiltinLedAddr;
}

void loop() {
  jonisk.update();
  // jTl.update();
}
