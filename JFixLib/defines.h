#ifndef JFIX_DEFINES_H
#define JFIX_DEFINES_H

#include <stdint.h>

#define WIFI_CHANNEL 1
#define JFIX_VERSION "2.0"

struct floatColor {
  float r;
  float g;
  float b;
  floatColor(float r = 0.0f, float g = 0.0f, float b = 0.0f) : r(r), g(g), b(b) {}
};

#endif // JFIX_DEFINES_H
