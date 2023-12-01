#include "JOsc.h"

JOsc::JOsc(JWavetable *w) { this->w = w; }

void JOsc::update() {
  // double t = (millis() - startTime) / 1000.; // To seconds
  int deltaTime =
      (millis() - syncTime) -
      lastUpdated; // Translate this to a increment, based on frequency
  // phase = t * frequency;
  phase += (deltaTime / 1000.) * frequency;
  phase = fmod(phase, 1.0);
  if (phase < 0) {
    phase = 1.0 - phase;
  }
  updateEnvelopes();
  checkLifeTime();
  lastUpdated = (millis() - syncTime);
}

void JOsc::start() {
  bActive = true;
  startTime = millis() - syncTime;
}

void JOsc::draw(floatColor **leds, int numLedsPerString, char numStrings,
                int horizontalPixelSpacing) {
  if (bWaitForEnv)
    return;

  float x =
      loc[0] * viewport[0] - (viewportOffset[0] * viewport[0]); // in Pixels
  float y = loc[1] * viewport[1] - (viewportOffset[1] * viewport[1]);
  float width = size[0] * viewport[0];
  float h = size[1] * viewport[1]; // in Pixels
  float xEnd = x + width;
  float yEnd = y + h;

  int xReadPositions[2] = {0, horizontalPixelSpacing}; // [0, 10]
  int hPixels =
      numLedsPerString * size[1]; // To do: use loc[0] to determine if in view
  int yStart = numLedsPerString * loc[1];
  for (int j = 0; j < numStrings; j++) {
    if (x > xReadPositions[j] || xEnd < xReadPositions[j] || yEnd < 0 ||
        x > viewport[0] || y > viewport[1])
      continue;

    if (x <= xReadPositions[j] && xEnd >= xReadPositions[j]) {
      if (y < viewport[1] || y <= 0) {
        if (y < 0)
          y = 0;
        if (yEnd > numLedsPerString)
          yEnd = numLedsPerString;
        for (int i = 0; i < hPixels; i++) {
          float pct = i / (float)hPixels;
          pct *= range;
          pct += offset + phase;
          pct = fmod(pct, 1.0);
          float v = w->getValue(pct); // Get by pct (0.0 - 0.99999)
          if (yStart + i >= numLedsPerString)
            continue;
          writeRGB(yStart + i, rgba[0] * v * brightness,
                   rgba[1] * v * brightness, rgba[2] * v * brightness, j, leds);
          // writeRGB(i, v, v, v, j, leds);
        }
      }
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
