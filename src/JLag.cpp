#include "JLag.h"
#include <Arduino.h>

JLag::JLag() {}

void JLag::update() {
  if (lagTime) {
    if (millis() < lagEndTime) {                         // lagEndTime
      float ratio = (millis() - lagStartTime) / lagTime; // 0.0 - 1.0
      *value = (startVal * (1.0 - ratio)) + (endVal * ratio);
    } else {
      if (!bLagDone) {
        *value = endVal;
        bLagDone = true;
      }
    }
  } else {
    *value = endVal;
  }
}

void JLag::set(float v) {
  lagStartTime = millis();
  lagEndTime = lagStartTime + lagTime;
  startVal = *value;
  endVal = v;
  bLagDone = false;
}

void JLag::link(float *v) { value = v; }
