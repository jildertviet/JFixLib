#include "JRect.h"

JRect::JRect(unsigned short lifeTime){
  if(lifeTime)
    endTime = millis() + lifeTime;
}

void JRect::draw(CRGB** leds, int numLedsPerString, char numStrings){
  if(canvas){
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
