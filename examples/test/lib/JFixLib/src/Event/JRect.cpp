#include "JRect.h"

JRect::JRect(unsigned short lifeTime){
  endTime = millis() + lifeTime;
}

JRect::JRect(unsigned char x, unsigned char y, unsigned short width, unsigned short height, unsigned char r, unsigned char g, unsigned char b, short lifeTime, float speedX, float speedY){
  loc[0] = x/255.;
  loc[1] = y/255.;
  size[0] = width;
  size[1] = height;
  // color[0] = r;
  // color[1] = g;
  // color[2] = b;
  color = RGBA8888(r, g, b, 255);
  endTime = millis() + lifeTime;
  speed[0] = speedX;
  speed[1] = speedY;
  Serial.print("endTime: "); Serial.println(endTime);
}

void JRect::draw(){
  if(canvas){
    // canvas->fillJRect(loc[0] * w, loc[1] * h, size[0], size[1], canvas->RGB(color[0], color[1], color[2]));
    canvas->setBrushColor(RGB888(color.R * brightness, color.G * brightness, color.B * brightness)); // Brightness gets calculated in checkLifeTime() of JEvent. Not very intuitive...
    canvas->fillRectangle(loc[0], loc[1], loc[0] + size[0], loc[1] + size[1]);
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
