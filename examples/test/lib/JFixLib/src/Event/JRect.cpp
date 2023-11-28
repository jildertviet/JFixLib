#include "JRect.h"

JRect::JRect(unsigned short lifeTime) {
  if (lifeTime)
    endTime = millis() + lifeTime;
  busses[0] = &bInvertHeight;
}

void JRect::draw(floatColor **leds, int numLedsPerString, char numStrings,
                 int horizontalPixelSpacing) {
  if (bWaitForEnv)
    return;
  float x =
      loc[0] * viewport[0] - (viewportOffset[0] * viewport[0]); // in Pixels
  float y = loc[1] * viewport[1] - (viewportOffset[1] * viewport[1]);
  float w = size[0] * viewport[0];
  float h = size[1] * viewport[1]; // in Pixels
  if (bInvertHeight)
    h = h * -1;
  float xEnd = x + w;
  float yEnd = y + h;
  if (yEnd < 0)
    yEnd = 0;

  int xReadPositions[2] = {0, horizontalPixelSpacing}; // [0, 10]
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
        for (int i = y; i < yEnd; i++) {
          // Serial.print("WriteRGB: "); Serial.println(i);
          writeRGB(i, rgba[0] * brightness, rgba[1] * brightness,
                   rgba[2] * brightness, j, leds);
        }
      }
    }
  }
}

void JRect::update() {
  updateEnvelopes();
  if (checkLifeTime()) {
    //
  }
  // if(speed[0] || speed[1]){
  //   loc[0] += speed[0];
  //   loc[1] += speed[1];
  //   if(loc[0] + (size[0]/(float)w) > 1.0 || loc[0] < 0.){
  //     if(limitMode == MODE_BOUNCE)
  //       speed[0] *= -1;
  //   }
  //   if(loc[1] + (size[1]/(float)h)> 1.0 || loc[1] < 0.){
  //     if(limitMode == MODE_BOUNCE)
  //       speed[1] *= -1;
  //   }
  // }
}
void JRect::setVal(char type, float value) {
  Event::setVal(type, value);
  switch (type) {
  // case 'h': size[1] = value; break;
  default:
    break;
  }
}
