#pragma once

#include "JEspnowDevice.h"
#include "Event/Event.h"
#include "Event/JEvent_Perlin.h"
#include "Event/JRect.h"
#include "fabgl.h"

#define MAX_EVENTS  128

fabgl::VGAController vga;
fabgl::Canvas canvas(&vga);

class JFixtureGraphics : public JEspnowDevice{
  public:
    JFixtureGraphics(){
      memset(events, 0x00, sizeof(events));
      // vga.begin(GPIO_NUM_20, GPIO_NUM_21, GPIO_NUM_25, GPIO_NUM_26, GPIO_NUM_2); // Fabgl doesn't seem to work without the VGA part
      vga.begin(GPIO_NUM_20, GPIO_NUM_21, GPIO_NUM_19, GPIO_NUM_25, GPIO_NUM_16, GPIO_NUM_17, GPIO_NUM_2, GPIO_NUM_4);
      vga.setResolution(VGA_320x200_75Hz, 100, 144);
      canvas.setBrushColor(Color::Black);
      canvas.clear();
      // canvas.setBrushColor(Color::Green);
      // canvas.fillRectangle(0, 0, 20, 100);
    }
    Event* lastAdded = nullptr;
    Event* events[MAX_EVENTS];
    float viewPort[2] = {20, 144};
    float viewPortOffset[2] = {0,0}; // Original
    
    void (*writeRGBPtr)(int, float, float, float, char, CRGB**) = nullptr; // Will point to function of inherited class (JFixtureAddr)

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
      Event* e = nullptr;
      switch(data[0]){
        case 0x01:{ // Perlin
          e = (Event*)new JEvent_Perlin();
// [noiseScale, noiseTimeScale, horizontalPixelDistance, horizontalPixelOffset].asBytes32F ++ pos.asBytes32F ++ size.asBytes32F ++ rgb ++ "end");
        }
        break;
        case 0x02:{ // JRect
          JRect* r = new JRect(); // lifeTime
          memcpy(&(r->loc), data+1, sizeof(float)*2);
          memcpy(&(r->size), data+1+(sizeof(float)*2), sizeof(float)*2);
          memcpy(&(r->rgba), data+1+(sizeof(float)*4), sizeof(float)*4);
          e = (Event*)r;
          // e->canvas = &canvas; // Only link this in Events that actually use the canvas
        }
        break;
      }
      if(e){
        e->viewPort = viewPort;
        e->viewPortOffset = viewPortOffset;
        e->writeRGB = this->writeRGBPtr; // Events use the static function of their parent to write to the LEDs
        addEvent(e);
      }
    }

    void addEvent(Event* e){
      for(char i=0; i<MAX_EVENTS; i++){
        if(events[i] == nullptr){
          e->bActive = true;
          // e->canvas = &canvas;
          events[i] = e;
          lastAdded = e; // Isn't set to null when Event gets deleted
          Serial.print("New event @: "); Serial.println((int)i);
          break;
        }
      }
    }

    void addEnv(const uint8_t *data, int data_len) override{
      float a, s, r, b;
      memcpy(&a, data, sizeof(float));
      memcpy(&s, data+(sizeof(float)*1), sizeof(float));
      memcpy(&r, data+(sizeof(float)*2), sizeof(float));
      memcpy(&b, data+(sizeof(float)*3), sizeof(float));
      char bKill = data[(sizeof(float)*4)]; // Currently always deletes when done
      char bType = data[(sizeof(float)*4) + 1];
      char key = data[(sizeof(float)*4) + 2]; // Get Env by key, instead of always last. 
      Event* e = lastAdded;
      switch(bType){
        // case 'b': e->triggerBrightnessEnv(a, s, r, b); break;
        case 'b': e->addEnv('b', &e->brightness, a, s, r, b, e->brightness); break;
        case 'x': e->addEnv('x', &e->loc[0], a, s, r, b, e->loc[0]); break;
        case 'y': e->addEnv('y', &e->loc[1], a, s, r, b, e->loc[1]); break;
        case 'w': e->addEnv('w', &e->size[0], a, s, r, b, e->size[0]); break;
        case 'h': e->addEnv('h', &e->size[1], a, s, r, b, e->size[1]); break;
      }
    }

    void deleteEvents(){
      for(int i=0; i<MAX_EVENTS; i++){
        if(events[i]){
          delete events[i];
          events[i] = nullptr;
        }
      }
    }
};
