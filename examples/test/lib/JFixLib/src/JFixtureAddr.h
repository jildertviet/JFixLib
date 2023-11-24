#pragma once
#include "FastLED.h"
#include "FastLedContstants.h"
#include "JFixture.h"
#include "JFixtureGraphics.h"
#include "ofNoise.h"
// Addressable leds
//
class JFixtureAddr : public JFixtureGraphics {
public:
  CRGB **leds = nullptr;
  CRGB **ledsToWrite = nullptr; // Use this array for the brightness curve. The
                                // other is for reading back
  float ***ledsValues = nullptr;
  float brightnessCurve[256];
  int numLedsPerString = 1;
  char numStrings = 1;

  float noiseScale = 0.01;
  float noiseTimeScale = 0.0005;
  int horizontalPixelDistance = 10; // Beams are 10 pixels from eachother

  JFixtureAddr(){};
  enum JAddressableMode { J_WS2812B, J_WS2816B };
  enum JDrawMode { TEST, TEST_PERLIN, LIVE };
  JDrawMode drawMode = LIVE;

  void setup(char numColorChannels = 6, const uint8_t *pins = nullptr,
             short numLedsPerString = 60, JAddressableMode mode = J_WS2812B,
             int numStrings = 1) {
    channels = new float[numColorChannels];
    this->numLedsPerString = numLedsPerString;
    this->numStrings = numStrings;
    if (mode == J_WS2812B) {
      leds = new CRGB *[numStrings];
      ledsToWrite = new CRGB *[numStrings];
      for (int i = 0; i < numStrings; i++) {
        leds[i] = new CRGB[numLedsPerString];
        ledsToWrite[i] = new CRGB[numLedsPerString];
        addLeds(pins[i], leds[i], numLedsPerString);
      }
    } else if (mode == J_WS2816B) {
      leds = new CRGB *[numStrings];
      ledsToWrite = new CRGB *[numStrings];
      // ledsValues = new float**[numStrings];
      for (int i = 0; i < numStrings; i++) {
        ledsToWrite[i] = new CRGB[numLedsPerString * 2];
        // ledsValues[i] = new float*[numLedsPerString];
        // for(int j=0; j<numLedsPerString; j++){
        // ledsValues[i][j] = new float[3];
        // memset(ledsValues[i][j], 0x00, sizeof(float)*3)
        // }
        addLeds(pins[i], ledsToWrite[i], numLedsPerString * 2);
      }
    }
    writeRGBPtr = &this->writeRGB;
    initCurve();
    testLED();
    allBlack(true);

    //  float r, g, b;
    //  r = 1.0; g = 0.5; b=0.1;
    //  uint16_t rgb[3];
    //  rgb[0] = r * 65535;
    //  rgb[1] = g * 65535;
    //  rgb[2] = b * 65535;
    //  char split[3][2];
    //  for(int i=0; i<3; i++){
    //    memcpy(split[i], &rgb[i], 2);
    //  }
    //  CRGB ledsTest[2];
    //  ledsTest[0] = CRGB(split[1][0], split[1][1], split[0][1]);
    //  ledsTest[1] = CRGB(split[2][1], split[0][0], split[2][0]);
    //
    // Serial.println(rTestF);
  }

  void initCurve() { // 0.0 -- 1.0
    for (int i = 0; i < 256; i++) {
      brightnessCurve[i] = pow((i / 256.), 2.0);
    }
  }

  void setLED(char channel, int value) { // Receives 0 - 255
  }

  static void writeRGB(int id, float r, float g, float b, char channel,
                       CRGB **leds) {
    if (!leds)
      return;

    uint16_t rgb[3];
    float rgbF[3];
    char split[3][2] = {
        {leds[channel][id * 2 + 1].g, leds[channel][id * 2 + 0].b},
        {leds[channel][id * 2 + 0].r, leds[channel][id * 2 + 0].g},
        {leds[channel][id * 2 + 1].b, leds[channel][id * 2 + 1].r},
    };
    for (int i = 0; i < 3; i++) {
      memcpy(&rgb[i], split[i], 2); // Read the values currently on the LEDs
      rgbF[i] = rgb[i] / 65535.;
    }

    if (r < rgbF[0])
      r = rgbF[0];
    if (g < rgbF[1])
      g = rgbF[1];
    if (b < rgbF[2])
      b = rgbF[2];
    if (r < e->rgbaBackground[0])
      r = e->rgbaBackground[0];
    if (g < e->rgbaBackground[1])
      g = e->rgbaBackground[1];
    if (b < e->rgbaBackground[2])
      b = e->rgbaBackground[2];
    // Check values in leds
    // Convert leds[2] to one RGB float pair
    // uint16_t rTest = 0;
    // char splitNew[2] = {ledsTest[0].r, ledsTest[0].g};
    // memcpy(&rTest, splitNew, 2);
    // float rTestF = rTest / 65535.;

    // rgb[0] = pow(r, 2.0) * 65535;
    // rgb[1] = pow(g, 2.0) * 65535;
    // rgb[2] = pow(b, 2.0) * 65535;
    rgb[0] = r * 65535;
    rgb[1] = g * 65535;
    rgb[2] = b * 65535;

    for (int i = 0; i < 3; i++) {
      memcpy(split[i], &rgb[i], 2);
    }

    leds[channel][(id * 2) + 0] = CRGB(split[1][0], split[1][1], split[0][1]);
    leds[channel][(id * 2) + 1] = CRGB(split[2][1], split[0][0], split[2][0]);
    // leds[channel][(id*2)+0] = CRGB(split[1][0], split[1][1], split[0][0]); //
    // Different, because FastLED set to WS2812B instead of NEOPIXEL
    // leds[channel][(id*2)+1] = CRGB(split[0][1], split[2][0], split[2][1]);
  }

  void writeRGBHard(int id, float r, float g, float b, char channel,
                    CRGB **leds) { // Doesn't check previous values
    if (!leds)
      return;

    if (r < e->rgbaBackground[0])
      r = e->rgbaBackground[0];
    if (g < e->rgbaBackground[1])
      g = e->rgbaBackground[1];
    if (b < e->rgbaBackground[2])
      b = e->rgbaBackground[2];

    uint16_t rgb[3];
    char split[3][2];
    rgb[0] = r * 65535;
    rgb[1] = g * 65535;
    rgb[2] = b * 65535;
    for (int i = 0; i < 3; i++) {
      memcpy(split[i], &rgb[i], 2);
    }
    leds[channel][(id * 2) + 0] = CRGB(split[1][0], split[1][1], split[0][1]);
    leds[channel][(id * 2) + 1] = CRGB(split[2][1], split[0][0], split[2][0]);
  }

  void testLED(){

  };
  void showSucces() override {}
  void show() override{

  };

  void setChannel(char i, float v) override {
    for (int i = 0; i < numChannels; i++) {
      channels[i] = v;
    }
  }
  void allBlack(bool bWrite = false) {
    for (int j = 0; j < numStrings; j++) {
      for (int i = 0; i < numLedsPerString; i++) {
        writeRGBHard(i, 0, 0, 0, j, leds);
      }
    }
    if (bWrite)
      writeLeds();
  }

  void writeLeds() {
    memcpy(ledsToWrite, leds, sizeof(leds));
    for (int j = 0; j < numStrings; j++) {
      for (int i = 0; i < numLedsPerString; i++) {
        ledsToWrite[j][i].r = pow(ledsToWrite[j][i].r, 2);
        ledsToWrite[j][i].g = pow(ledsToWrite[j][i].g, 2);
        ledsToWrite[j][i].b = pow(ledsToWrite[j][i].b, 2);
      }
    }
    FastLED.show();
  }

  int ledIndex = 0;
  void update() override {
    JFixture::update();
    switch (drawMode) {
    case TEST: {
      allBlack(false);
      float v = sin(millis() * 0.001) * 0.5 + 0.5;
      writeRGB(10, v, 0.0, 0.0, 0, leds);
      writeRGB(10, 0, v, 0, 1, leds);
      writeRGB(20, 0, 0, v, 0, leds);
      // ledIndex = (ledIndex + 1) % numLedsPerString;
      writeLeds();
    } break;
    case TEST_PERLIN: {
      // noiseScale = 0.00001;
      // noiseTimeScale = 0.000001;
      for (float j = 0; j < numStrings; j++) {
        for (float i = 0; i < numLedsPerString; i++) {
          float val = ofNoise(i * noiseScale,
                              (j * horizontalPixelDistance) * noiseScale,
                              millis() * noiseTimeScale);
          val = pow(val, 2.0);
          if (val < 0.005)
            val = 0.005;
          writeRGB(i, 0.0, 0.0, val, j, leds);
        }
      }
      writeLeds();
    } break;
    case LIVE: {
      // JFixtureGraphics::update(); // Update Events, currently happening below
      // bool bUseCanvas = false;
      // for(int i=0; i<MAX_EVENTS; i++){
      // if(!events[i])
      // continue;
      // if(events[i]->bActive && events[i]->canvas){
      // bUseCanvas = true;
      // break;
      // }
      // }
      // if(bUseCanvas){
      // canvas.setBrushColor(RGB888(0,0,0)); // Brightness gets calculated in
      // checkLifeTime() of JEvent. Not very intuitive... canvas.clear();
      // }
      parseMsgs();
      allBlack();
      bool bEventUpdate = false;
      for (char i = 0; i < MAX_EVENTS; i++) {
        if (events[i]) {
          events[i]->update();
          if (events[i]->bActive) {
            bEventUpdate = true;
            events[i]->draw(
                leds, numLedsPerString, numStrings,
                horizontalPixelDistance); // Write to LEDs, or canvas
          } else {
            Serial.print("Delete ");
            Serial.println((int)events[i]->id);
            delete events[i];
            events[i] = nullptr;
          }
        }
      }
      writeLeds();
      if (bEventUpdate) {
        delayMicroseconds(1500); // Or even 0?
      } else {
        delay(2);
      }
      // if(bUseCanvas){
      // canvas.waitCompletion();
      // canvasToLeds();
      // }
    } break;
    }
    // delayMicroseconds(500);
  }

  void canvasToLeds() {
    for (int x = 0; x < numStrings; x++) {
      for (int y = 0; y < numLedsPerString; y++) {
        RGB888 color =
            canvas.getPixel(x * horizontalPixelDistance, y); // Read the pixel
        // Serial.println(color.R);
        writeRGB(y, color.R / 255., color.G / 255., color.B / 255., x, leds);
      }
    }
  }
  void blink(char num = 1, short dur = 100, short delayTime = 100,
             char channel = 0) override {
    float values[3] = {0};
    values[channel] = 1.0;
    writeRGB(1, values[0], values[1], values[2], 0, leds);
  }
};
