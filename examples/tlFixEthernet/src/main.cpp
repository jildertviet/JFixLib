#define WS2812B_STATUS_LED
// char version[2] = {1, 1};
#include "JFixLib.h"

JTlFixEthernet jTl;

void setup() {
  JTlFixSettings settings;
  settings.networkName = "JV_";
  settings.pins = new uint8_t[2]{22, 23}; // Test on pin3
  jTl.setup(settings);
}

void loop() { jTl.update(); }
