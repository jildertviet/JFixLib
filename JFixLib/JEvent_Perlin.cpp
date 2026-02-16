#include "JEvent_Perlin.h"
#include "ofNoise.h"
#include <cmath>

JEvent_Perlin::JEvent_Perlin() {
  customBusses[0] = &noiseScale;
  customBusses[1] = &noiseTimeScale;
  customBusses[2] = &horizontalPixelOffset;
}

void JEvent_Perlin::update() {
  Event::update();
}

void JEvent_Perlin::draw(floatColor **leds, int numLedsPerString,
                         char numStrings, int horizontalPixelDistance) {
  if (leds && brightness > 0) {
    float y = loc[1] * viewport[1] - (viewportOffset[1] * viewport[1]);
    float h = size[1] * viewport[1];
    float yEnd = y + h;

    uint32_t t = getMillis() - syncTime;
    
    for (int j = 0; j < (int)numStrings; j++) {
      if (yEnd < 0 || y > viewport[1])
        continue;
      
      float currentY = (y < 0) ? 0 : y;
      float currentYEnd = (yEnd > numLedsPerString) ? (float)numLedsPerString : yEnd;

      for (float i = currentY; i < currentYEnd; i++) {
        float val = ofNoise(i * noiseScale,
                            ((j * horizontalPixelDistance) + horizontalPixelOffset) * noiseScale,
                            (float)t * noiseTimeScale);
        
        val = val * val; // Square for better look
        
        if (writeRGB) {
          writeRGB((int)i, 
                   val * brightness * rgba[0], 
                   val * brightness * rgba[1],
                   val * brightness * rgba[2], 
                   (uint8_t)j, leds);
        }
      }
    }
  }
}
