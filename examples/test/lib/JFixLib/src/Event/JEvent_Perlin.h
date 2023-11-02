#pragma once

#include "Event/Event.h"
#include "ofNoise.h"

class JEvent_Perlin : public Event{
  public:
    JEvent_Perlin(){};
    // JFixtureAddr* parent = nullptr;
    float noiseScale = 0.01;
    float noiseTimeScale = 0.0005;
    int horizontalPixelDistance = 10; // Beams are 10 pixels from eachother

    void update(){
      // checkLifetime();
    }

    void draw(CRGB** leds, int numLedsPerString, char numStrings) override{
      // Should write to FastLED?
      if(leds){
        for(float j=0; j<numStrings; j++){
          for(float i=0; i<numLedsPerString; i++){
            float val = ofNoise(i*noiseScale, (j*horizontalPixelDistance)*noiseScale, millis() * noiseTimeScale);
            val = pow(val, 2.0);
            if(val < 0.005)
              val = 0.005;
            if(writeRGB)
              writeRGB(i, 0.0, 0.0, val, j, leds);
          }
        }
 
      }
    }
};
