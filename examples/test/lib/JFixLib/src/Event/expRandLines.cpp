#include "expRandLines.h"

expRandLines::expRandLines(unsigned char x, unsigned char y, unsigned short width, unsigned short height, unsigned char r, unsigned char g, unsigned char b, short lifeTime, unsigned short delayTime, unsigned char sizeRange){
    loc[0] = x/255.;
    loc[1] = y/255.;
    size[0] = width;
    size[1] = height;
    // color[0] = r;
    // color[1] = g;
    // color[2] = b;
    color = RGBA8888(r, g, b, 255);
    endTime = millis() + lifeTime;
    this->delayTime = delayTime;
    this->sizeRange = sizeRange;
//    Serial.print("delayTime: "); Serial.println(delayTime);
}

void expRandLines::update(){
    checkLifeTime();
    if(millis() > lastTriggered + delayTime){
//        Serial.println("New parameters");
        lastTriggered = millis();
        // Get new X, Y, W and H
        if(random(0,2) >= 1){
            size[0] = w;
            size[1] = pow(random(1000)/1000., 4) * sizeRange + 1;
            loc[0] = 0;
            loc[1] = 0.5 + (pow(random(1000)/1000., 4) * 0.5) * (random(2)*2-1); // + offset
        } else{
            size[0] = pow(random(1000)/1000., 4) * sizeRange + 1;
            size[1] = h;
            loc[0] = 0.5 + (pow(random(1000)/1000., 4) * 0.5) * (random(2)*2-1); // + offset
            loc[1] = 0;
        }
    }
}

void expRandLines::draw(){
    if(canvas){
        // canvas->fillRect(loc[0] * w, loc[1] * h, size[0], size[1], canvas->RGB(color[0], color[1], color[2]));
    }
}
