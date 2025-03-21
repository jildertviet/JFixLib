// char version[2] = {1, 1};
#include "JFixLib.h"

Jllll jllll;

void setup() {
  JllllSettings settings;
  settings.networkName = "JV_";
  settings.pins = new uint8_t[1]{22}; // Test on pin3
  jllll.setup(settings);
  Serial.println(xPortGetCoreID());
}

void loop() { jllll.update(); }
