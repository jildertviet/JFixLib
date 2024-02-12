#define WS2812B_STATUS_LED
// char version[2] = {1, 1};
#define ADAFRUIT_NUM_PIXELS 60
#define ADAFRUIT_DATA_PIN 4

#include "JFixLib.h"

JLedBeam b;

void setup() {
  JLedBeamSettings settings;
  settings.networkName = "JV_";
  settings.pins = new uint8_t[1]{4}; // Test on pin3
  b.setup(settings);
}

void loop() { b.update(); }
