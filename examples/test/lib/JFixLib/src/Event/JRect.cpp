#include "JRect.h"

JRect::JRect(unsigned short lifeTime){
  if(lifeTime)
    endTime = millis() + lifeTime;
}

void JRect::draw(CRGB** leds, int numLedsPerString, char numStrings, int horizontalPixelSpacing){
  if(canvas || 1 == 0){ // Disabled
    float w = canvas->getWidth();
    float h = canvas->getHeight();
    // Serial.println(brightness);
    // canvas->fillJRect(loc[0] * w, loc[1] * h, size[0], size[1], canvas->RGB(color[0], color[1], color[2]));
    // Serial.println(brightness);
    unsigned char r = rgba[0] * brightness * 255.0;
    // Serial.println(r)
    canvas->setBrushColor(RGB888(r, rgba[1] * brightness * 255, rgba[2] * brightness * 255)); // Brightness gets calculated in checkLifeTime() of JEvent. Not very intuitive...
    canvas->fillRectangle(loc[0] * w, loc[1] * (brightness * 50), (loc[0] + size[0])*w, (loc[1] + size[1])*h);
  }
  float x = loc[0] * viewPort[0]; // in Pixels
  float y = loc[1] * viewPort[1];
  float w = size[0] * viewPort[0];
  float h = size[1] * viewPort[1]; // in Pixels
  float xEnd = x + w;
  float yEnd = y + h;
  
  // Serial.print("X: "); Serial.println(x);
  // Serial.print("Y: "); Serial.println(y);
  // Serial.print("W: "); Serial.println(w);
  // Serial.print("H: "); Serial.println(h);
  // Serial.print("xEnd: "); Serial.println(xEnd);
  // Serial.print("yEnd: "); Serial.println(yEnd);
  // tlFix specific :(
  // if(x <= 0 || x < viewPort[0] && y <= 0 || y < viewPort[y]){
    // Origin of rect lies within viewPort
  // }
  int xReadPositions[2] = {0, horizontalPixelSpacing}; // [0, 10]
  for(int j=0; j<numStrings; j++){
    if(x > xReadPositions[j] || xEnd < xReadPositions[j] || yEnd < 0 || x > viewPort[0] || y > viewPort[1])
      continue;
    if(x <= xReadPositions[j] && xEnd >= xReadPositions[j]){
      if(y < viewPort[1] || y <= 0){
        if(y < 0)
          y = 0;
        if(yEnd > numLedsPerString)
          yEnd = numLedsPerString;
        for(int i=y; i<yEnd; i++){
          // Serial.print("WriteRGB: "); Serial.println(i);
          writeRGB(i, rgba[0] * brightness, rgba[1] * brightness, rgba[2] * brightness, j, leds);
        }
      }
    }
  }
  // writeRGB(10, rgba[0] * brightness, rgba[1] * brightness, rgba[2] * brightness, 0, leds);
}

void JRect::update(){
  if(checkLifeTime()){
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
