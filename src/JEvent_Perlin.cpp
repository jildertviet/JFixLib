#include "JEvent_Perlin.h"
#include "ofNoise.h"

JEvent_Perlin::JEvent_Perlin() {
  busses[0] = &noiseScale;
  busses[1] = &noiseTimeScale;
  busses[2] = &horizontalPixelOffset;
}

void JEvent_Perlin::update() {
  Event::update(); // check parameterMappers
  updateEnvelopes();
  checkLifeTime();
}

void JEvent_Perlin::draw(floatColor **leds, int numLedsPerString,
                         char numStrings, int horizontalPixelDistance) {
  if (leds && brightness) {
    float y =
        loc[1] * viewport[1] - (viewportOffset[1] * viewport[1]); // In pixels
    float h = size[1] * viewport[1];                              // in Pixels
    float yEnd = y + h;

    unsigned long t = millis() - syncTime;
    for (float j = 0; j < numStrings; j++) {
      if (yEnd < 0 || y > viewport[1])
        continue;
      if (y < 0) // Clip in range
        y = 0;
      if (yEnd > numLedsPerString)
        yEnd = numLedsPerString;

      for (float i = y; i < yEnd; i++) {
        float val =
            ofNoise(i * noiseScale,
                    ((j * horizontalPixelDistance) + horizontalPixelOffset) *
                        noiseScale,
                    t * noiseTimeScale);
        // val = pow(
        // val, 2.0); // Already @ writeRGB. Edit; do it anyway, looks
        // better
        val = val * val;
        // if (val < 0.005) // Do this with a background
        // val = 0.005;
        if (writeRGB)
          writeRGB(i, val * brightness * rgba[0], val * brightness * rgba[1],
                   val * brightness * rgba[2], j, leds);
      }
    }
  }
}
