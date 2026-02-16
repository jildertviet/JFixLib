#include "JWavetable.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

JWavetable::JWavetable(int resolution) {
  this->resolution = resolution;
  data = new float[resolution];
}

JWavetable::~JWavetable() {
  delete[] data;
}

void JWavetable::fillSineNorm(float power) {
  for (int i = 0; i < resolution; i++) {
    float pct = i / (float)resolution;
    // sin returns -1 to 1, scale to 0 to 1
    data[i] = std::sin(2.0f * M_PI * pct) * 0.5f + 0.5f;
    if (power != 1.0f) {
      data[i] = std::pow(data[i], power);
    }
  }
}

float JWavetable::getValue(float pct) {
  if (pct < 0) pct = 0;
  if (pct >= 1.0f) pct = 0.999f;
  return data[(int)(pct * resolution)];
}
