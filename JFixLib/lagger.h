#ifndef LAGGER_H
#define LAGGER_H

#include <stdint.h>

class Lagger {
public:
  Lagger();
  void update();
  void set(float v);
  void link(float *v);

  float lagTime = 1000.0f; // ms
  uint32_t lagStartTime = 0;
  uint32_t lagEndTime = 0;
  float startVal = 0.0f;
  float endVal = 0.0f;
  float *value = nullptr;
  bool bLagDone = true;

private:
  uint32_t getMillis();
};

#endif // LAGGER_H
