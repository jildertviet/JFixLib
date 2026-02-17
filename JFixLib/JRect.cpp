#include "JRect.h"
#include <cmath>

JRect::JRect(uint16_t lifeTime) {
  if (lifeTime) {
    endTime = getMillis() + lifeTime;
  }
  customBusses[0] = &bInvertHeight;
}

void JRect::draw(floatColor **leds, int numLedsPerString, char numStrings,
                 int horizontalPixelSpacing) {
  if (bWaitForEnv || brightness == 0 || !leds || !writeRGB)
    return;

  float x = loc[0] * viewport[0] - (viewportOffset[0] * viewport[0]);
  float y = loc[1] * viewport[1] - (viewportOffset[1] * viewport[1]);
  float w = size[0] * viewport[0];
  float h = size[1] * viewport[1];

  if (bInvertHeight > 0.5f) {
    h = h * -1.0f;
  }

  float xEnd = x + w;
  float yEnd = y + h;

  if (yEnd < 0) yEnd = 0;

  float currentY = y;
  float currentYEnd = yEnd;

  if (bInvertHeight > 0.5f) {
    currentY = yEnd;
    currentYEnd = y;
  }

  // Support for 2 strings / beams
  int xReadPositions[2] = {0, horizontalPixelSpacing};
  
  for (int j = 0; j < (int)numStrings; j++) {
    if (j >= 2) break; // Based on original logic having xReadPositions[2]

    if (x > xReadPositions[j] || xEnd < xReadPositions[j] || currentYEnd < 0 ||
        x > viewport[0] || currentY > viewport[1])
      continue;

    if (x <= xReadPositions[j] && xEnd >= xReadPositions[j]) {
      float renderY = (currentY < 0) ? 0 : currentY;
      float renderYEnd = (currentYEnd > numLedsPerString) ? (float)numLedsPerString : currentYEnd;

      for (float i = renderY; i < renderYEnd; i++) {
        writeRGB((int)i, 
                 rgba[0] * brightness, 
                 rgba[1] * brightness,
                 rgba[2] * brightness, 
                 (uint8_t)j, leds);
      }
    }
  }
}

void JRect::update() {
  Event::update();
}

void JRect::setVal(char type, float value) {
  Event::setVal(type, value);
}
