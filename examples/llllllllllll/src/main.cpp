// char version[2] = {1, 1};
#include "JFixLib.h"

Jllll jllll;

void setup() {
  JllllSettings settings;
  settings.networkName = "JV_";
  settings.pins = new uint8_t[1]{22}; // Test on pin3
  jllll.setup(settings);
}

void loop() { jllll.update(); }

extern "C" void app_main(void) {
  setup();
  while (1) {
    loop();
  }
}
