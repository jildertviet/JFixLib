#include "JOsc.h"

JOsc::JOsc(JWavetable *w) { this->w = w; }

void JOsc::update() {
  double t = (millis() - startTime) / 1000.; // To seconds
  int deltaTime =
      millis() -
      lastUpdated; // Translate this to a increment, based on frequency
  // phase = t * frequency;
  phase += (deltaTime / 1000.) * frequency;
  phase = fmod(phase, 1.0);
  if (phase < 0) {
    phase = 1.0 - phase;
  }
  updateEnvelopes();
  checkLifeTime();
}

void JOsc::start() {
  bActive = true;
  startTime = millis();
}

void JOsc::draw(CRGB **leds, int numLedsPerString, char numStrings,
                int horizontalPixelDistance) {
  if (bWaitForEnv)
    return;

  int hPixels =
      numLedsPerString * size[1]; // To do: use loc[0] to determine if in view
  int xStart = numLedsPerString * loc[1];
  for (int j = 0; j < numStrings; j++) {
    for (int i = 0; i < hPixels; i++) {
      float pct = i / (float)hPixels;
      pct *= range;
      pct += offset + phase;
      pct = fmod(pct, 1.0);
      float v = w->getValue(pct); // Get by pct (0.0 - 0.99999)
      if (xStart + i >= numLedsPerString)
        continue;
      writeRGB(xStart + i, rgba[0] * v * brightness, rgba[1] * v * brightness,
               rgba[2] * v * brightness, j, leds);
      // writeRGB(i, v, v, v, j, leds);
    }
  }
}

void JOsc::setWavetable(JWavetable *w) { this->w = w; }

void JOsc::setVal(char type, float value) {
  Event::setVal(type, value);
  switch (type) {
  case 'f':
    frequency = value;
    break;
  case 'r':
    range = value;
    break;
  case 'o':
    offset = value;
    break;
  case 'q':
    w->fillSineNorm(value);
    break;
  }
}
