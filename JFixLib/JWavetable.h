#ifndef WAVETABLE_H
#define WAVETABLE_H

class JWavetable {
public:
  JWavetable(int resolution);
  ~JWavetable();
  int resolution;
  void fillSineNorm(float power = 1.0f);
  float *data;
  float getValue(float pct);
};

#endif // WAVETABLE_H
