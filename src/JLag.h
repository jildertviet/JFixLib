#pragma once

class JLag {
public:
  float lagTime = 1000; // ms
  unsigned long lagStartTime = 0;
  unsigned long lagEndTime = 0;
  float startVal = 0;
  float endVal = 0; // Perhaps call this 'targetVal'
  float *value;
  bool bLagDone = true;

  JLag();
  void update();
  void set(float v);
  void link(float *v);
};
