#pragma once
#define CHANNEL 1

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
