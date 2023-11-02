#pragma once
#include "JFixture.h"
#include "JFixtureGraphics.h"
#include "FastLED.h"
#include "FastLedContstants.h"
#include "ofNoise.h"
// Addressable leds 
//
class JFixtureAddr: public JFixtureGraphics{
  public: 
  CRGB** leds = nullptr;
  float brightnessCurve[256];
  int numLedsPerString = 1;
  char numStrings = 1;

  float noiseScale = 0.01;
  float noiseTimeScale = 0.0005;
  int horizontalPixelDistance = 10; // Beams are 10 pixels from eachother

  JFixtureAddr(){};
  enum JAddressableMode{
    J_WS2812B,
    J_WS2816B
  };
  enum JDrawMode{
    TEST,
    TEST_PERLIN,
    LIVE
  };
  JDrawMode drawMode = LIVE;

  void setup(char numColorChannels = 6, const uint8_t* pins = nullptr, short numLedsPerString = 60, JAddressableMode mode = J_WS2812B, int numStrings = 1){
    channels = new float[numColorChannels];
    this->numLedsPerString = numLedsPerString;
    this->numStrings = numStrings;
    if(mode == J_WS2812B){
      leds = new CRGB*[numStrings];
      for(int i=0; i<numStrings; i++){
        leds[i] = new CRGB[numLedsPerString];
        addLeds(pins[i], leds[i], numLedsPerString);
      }
    } else if(mode == J_WS2816B){
      leds = new CRGB*[numStrings];
      for(int i=0; i<numStrings; i++){
        leds[i] = new CRGB[numLedsPerString * 2];
        addLeds(pins[i], leds[i], numLedsPerString * 2);
      }
    }
    writeRGBPtr = &this->writeRGB;
    initCurve();
    testLED();
    allBlack(true);
  }

  void initCurve(){ // 0.0 -- 1.0
    for(int i=0; i<256; i++){
      brightnessCurve[i] = pow((i / 256.), 2.0);
    }
  }

  void setLED(char channel, int value){ // Receives 0 - 255
  
  }

  static void writeRGB(int id, float r, float g, float b, char channel, CRGB** leds){
    if(!leds)
      return;
    uint16_t rgb[3];
    rgb[0] = r * 65535;
    rgb[1] = g * 65535;
    rgb[2] = b * 65535;
    char split[3][2];
    for(int i=0; i<3; i++){
      memcpy(split[i], &rgb[i], 2);
    }

    leds[channel][(id*2)+0] = CRGB(split[1][0], split[1][1], split[0][1]);
    leds[channel][(id*2)+1] = CRGB(split[2][1], split[0][0], split[2][0]);
    // leds[channel][(id*2)+0] = CRGB(split[1][0], split[1][1], split[0][0]); // Different, because FastLED set to WS2812B instead of NEOPIXEL
    // leds[channel][(id*2)+1] = CRGB(split[0][1], split[2][0], split[2][1]);
  }

  void testLED(){

  };
  void showSucces() override{
  }
  void show() override{
  
  };

  void setChannel(char i, float v) override{
    for(int i=0; i<numChannels; i++){
      channels[i] = v;
    }
  }
  void allBlack(bool bWrite = false){
    for(int j=0; j<numStrings; j++){
      for(int i=0; i<numLedsPerString; i++){
        writeRGB(i, 0, 0, 0, j, leds);
      }
    }
    if(bWrite)
      FastLED.show();
  }

  int ledIndex = 0;
  void update() override{
    JFixture::update();
    switch(drawMode){
      case TEST:{
        allBlack(false);
        float v = sin(millis() * 0.001) * 0.5 + 0.5;
        writeRGB(10, v, 0.0, 0.0, 0, leds);
        writeRGB(10, 0, v, 0, 1, leds);
        writeRGB(20, 0, 0, v, 0, leds);
        // ledIndex = (ledIndex + 1) % numLedsPerString;
        FastLED.show();                                       
      }
      break;
      case TEST_PERLIN:{
        // noiseScale = 0.00001;
        // noiseTimeScale = 0.000001;
        for(float j=0; j<numStrings; j++){
          for(float i=0; i<numLedsPerString; i++){
            float val = ofNoise(i*noiseScale, (j*horizontalPixelDistance)*noiseScale, millis() * noiseTimeScale);
            val = pow(val, 2.0);
            if(val < 0.005)
              val = 0.005;
            writeRGB(i, 0.0, 0.0, val, j, leds);
          }
        }
        FastLED.show();
      }
      break;
      case LIVE:{
        // JFixtureGraphics::update(); // Update Events                                
        bool bUseCanvas = false;
        for(int i=0; i<MAX_EVENTS; i++){
          if(!events[i])
            continue;
          if(events[i]->bActive && events[i]->canvas){
            bUseCanvas = true;
            break;
          }
        }
        if(bUseCanvas){
          canvas.setBrushColor(RGB888(0,0,0)); // Brightness gets calculated in checkLifeTime() of JEvent. Not very intuitive...
          canvas.clear();
        }
        for(char i=0; i<MAX_EVENTS; i++){
          if(events[i]){
            events[i]->update();
            if(events[i]->bActive){
             // Serial.print("Active event: "); Serial.println((int)i);
              events[i]->draw(leds, numLedsPerString, numStrings); // Write to LEDs, or canvas
            } else{
              Serial.println("delete events[i];");
              delete events[i];
              Serial.println("events[i] = nullptr;");
              events[i] = nullptr;
              Serial.println("done");
           }
        }
      }
      if(bUseCanvas){
        canvas.waitCompletion();
        canvasToLeds();
      }
      FastLED.show();
    }
      break;
    }
    delay(1);
  }

  void canvasToLeds(){
    for(int x=0; x<numStrings; x++){
      for(int y=0; y<numLedsPerString; y++){
        RGB888 color = canvas.getPixel(x*horizontalPixelDistance, y); // Read the pixel
        // Serial.println(color.R);
        writeRGB(y, color.R / 255., color.G / 255., color.B / 255., x, leds);
      }
    }
  } 
};
