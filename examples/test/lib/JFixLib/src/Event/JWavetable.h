#ifndef WAVETABLE_J
#define WAVETABLE_J

class JWavetable{
  public:
  JWavetable(int resolution);
  ~JWavetable();
  int resolution;
  void fillSineNorm(float power = 1);
  float* data;
  float getValue(float pct); 
};

#endif
