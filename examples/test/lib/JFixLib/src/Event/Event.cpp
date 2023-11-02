#include "Event.h"

Event::Event(){

}

void Event::testFunc(){

};

Event::~Event(){
//  Serial.println("Begin ~Event");
//  delete[] loc;
//  delete[] size;
//  delete[] color;
//  delete[] speed;
//  Serial.println("End ~Event");
}

bool Event::checkLifeTime(){
//  Serial.print("Millis: "); Serial.println(millis());
//  Serial.print("endTime: "); Serial.println(endTime);

  if(brightnessEnv.state != IDLE)
    brightness = brightnessEnv.update();
  if(endTime && millis() > endTime || brightnessEnv.state == DONE){
    bActive = false;
    return false;
  } else{
    return true;
  }
}

void Event::triggerBrightnessEnv(unsigned short a, unsigned short s, unsigned short r, float b){
  brightnessEnv.trigger(a, s, r, b);
}
