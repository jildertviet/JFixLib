#pragma once

#include "JEspnowDevice.h"
#include "Event/Event.h"

#define MAX_EVENTS  128
class JFixtureGraphics : public JEspnowDevice{
  public:
    JFixtureGraphics(){};
    Event* lastAddedTemp = nullptr;
    Event* events[MAX_EVENTS];
    void update() override{
      for(char i=0; i<MAX_EVENTS; i++){
        if(events[i]){
          events[i]->update();
          if(events[i]->bActive){
            Serial.print("Active event: "); Serial.println((int)i);
            events[i]->draw();
          } else{
            Serial.println("delete events[i];");
            delete events[i];
            Serial.println("events[i] = nullptr;");
            events[i] = nullptr;
            Serial.println("done");
          }
        }
      }
    }
};
