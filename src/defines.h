#ifndef JFIXDEFINES
#define CHANNEL 1
#define VERSION "11"

// A 0 isn't printed in SC, so use 1.1 as first version :)

struct floatColor {
  float r;
  float g;
  float b;
  floatColor(float r = 0, float g = 0, float b = 0) {
    this->r = r;
    this->g = g;
    this->b = b;
  }
};
#define JFIXDEFINES
#endif
