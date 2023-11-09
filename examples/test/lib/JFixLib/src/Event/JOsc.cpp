#include "JOsc.h"

JOsc::JOsc(JWavetable* w){
  this->w = w;
}

void JOsc::update(){
  double t = (millis() - startTime) / 1000.; // To seconds
  phase = t * frequency;
  phase = fmod(phase, 1.0);
  updateEnvelopes();
  checkLifeTime();
}

void JOsc::start(){
  bActive = true;
  startTime = millis();
}

void JOsc::draw(CRGB** leds, int numLedsPerString, char numStrings, int horizontalPixelDistance){
  if(bWaitForEnv)
    return;

  for(int j=0; j<numStrings; j++){
    for(int i=0; i<numLedsPerString; i++){
      float pct = i / (float)numLedsPerString;
      pct *= range;
      pct += offset + phase;
      pct = fmod(pct, 1.0);
      float v = w->getValue(pct); // Get by pct (0.0 - 0.99999)
      writeRGB(i, rgba[0] * v * brightness, rgba[1] * v * brightness, rgba[2] * v * brightness, j, leds);
      // writeRGB(i, v, v, v, j, leds);
    }
  }
}

void JOsc::setWavetable(JWavetable* w){
  this->w = w;
}

void JOsc::setVal(char type, float value){
  Event::setVal(type, value);
  switch(type){
    case 'f': frequency = value; break;
    case 'r': range = value; break;
    case 'o': offset = value; break;
    case 'q': w->fillSineNorm(value); break;
  }
}
