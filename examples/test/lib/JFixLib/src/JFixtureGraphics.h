#pragma once

#include "JEspnowDevice.h"
#include "Event/Event.h"
#include "Event/JEvent_Perlin.h"

#define MAX_EVENTS  128
class JFixtureGraphics : public JEspnowDevice{
  public:
    JFixtureGraphics(){
      for(int i=0; i<MAX_EVENTS; i++){
        events[i] = nullptr;
      }
    };
    Event* lastAddedTemp = nullptr;
    Event* events[MAX_EVENTS];

    void (*writeRGBPtr)(int, float, float, float, char, CRGB**) = nullptr;

    // void update() override{
    //   for(char i=0; i<MAX_EVENTS; i++){
    //     if(events[i]){
    //       events[i]->update();
    //       if(events[i]->bActive){
    //         // Serial.print("Active event: "); Serial.println((int)i);
    //         events[i]->draw(leds, numLedsPerString, numStrings);
    //         ...
    //       } else{
    //         Serial.println("delete events[i];");
    //         delete events[i];
    //         Serial.println("events[i] = nullptr;");
    //         events[i] = nullptr;
    //         Serial.println("done");
    //       }
    //     }
    //   }

    void addEvent(const uint8_t *data, int data_len) override{
      switch(data[0]){
        case 0x01:{ // Perlin
          Event* perlin = (Event*)new JEvent_Perlin();
          ((JEvent_Perlin*)perlin)->writeRGB = this->writeRGBPtr;
// [noiseScale, noiseTimeScale, horizontalPixelDistance, horizontalPixelOffset].asBytes32F ++ pos.asBytes32F ++ size.asBytes32F ++ rgb ++ "end");
         addEvent(perlin);       
        }
        break;
      }
    }

    void addEvent(Event* e){
      for(char i=0; i<MAX_EVENTS; i++){
        if(events[i] == nullptr){
          e->bActive = true;
          // e->canvas = &canvas;
          events[i] = e;
          lastAddedTemp = e; // Isn't set to null when Event gets deleted
          Serial.print("New event @: "); Serial.println((int)i);
          break;
        }
      }
    }
};
