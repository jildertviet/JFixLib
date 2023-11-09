#include "Arduino.h" // For TWO_PI
#include "JWavetable.h"

JWavetable::JWavetable(int resolution){
  this->resolution = resolution;
  data = new float[resolution];
}

JWavetable::~JWavetable(){
  delete data;
}

void JWavetable::fillSineNorm(float power){
  for(int i=0; i<resolution; i++){
    float pct = i / (float)resolution;
    data[i] = sin(TWO_PI * pct) * 0.5 + 0.5; // Normalized
    data[i] = pow(data[i], power);
  }
}

float JWavetable::getValue(float pct){
  return data[(int)(pct * resolution)];
}
