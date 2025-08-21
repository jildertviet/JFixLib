#include "JFixtureAddr.h"

NeoPixelBus<NeoGrb48Feature, NeoEsp32I2s1X8Ws2816Method>
    strip(NEOPIXELBUS_NUMLEDS, NEOPIXELBUS_PIN);

JFixtureAddr::JFixtureAddr(){};

void JFixtureAddr::setup(char numColorChannels, const uint8_t *pins,
                         short numLedsPerString, JAddressableMode mode,
                         int numStrings) {
  channels = new float[numColorChannels];
  ledMode = mode;
#ifdef NEOPIXELBUS
  numLedsPerString = NEOPIXELBUS_NUMLEDS;
#endif
  this->numLedsPerString = numLedsPerString;
  this->viewport[1] = numLedsPerString;
  this->numStrings = numStrings;
  if (mode == J_WS2812B) {
    leds = new floatColor *[numStrings];
    ledsToWrite = new CRGB *[numStrings];
    for (int i = 0; i < numStrings; i++) {
      leds[i] = new floatColor[numLedsPerString];
      ledsToWrite[i] = new CRGB[numLedsPerString];
#ifdef JFIX_FASTLED
      addLeds(pins[i], ledsToWrite[i], numLedsPerString);
#endif
    }
  } else if (mode == J_WS2816B) {
    leds = new floatColor *[numStrings];
    ledsToWrite = new CRGB *[numStrings];
    // ledsValues = new float**[numStrings];
    for (int i = 0; i < numStrings; i++) {
      leds[i] = new floatColor[numLedsPerString];
      ledsToWrite[i] = new CRGB[numLedsPerString * 2];
      // ledsValues[i] = new float*[numLedsPerString];
      // for(int j=0; j<numLedsPerString; j++){
      // ledsValues[i][j] = new float[3];
      // memset(ledsValues[i][j], 0x00, sizeof(float)*3)
#ifdef JFIX_FASTLED
      // pixels = new Adafruit_NeoPixel(numLedsPerString, pins[0],
      // NEO_GRB + NEO_KHZ800);
      addLeds(pins[i], ledsToWrite[i], numLedsPerString * 2);
#endif
      // }
    }
  }
  writeRGBPtr = &this->writeRGB;
  initCurve();
#ifdef NEOPIXELBUS
  Serial.println("Use NeoPixelBus");
  strip.Begin();
#endif
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

void JFixtureAddr::initCurve() { // 0.0 -- 1.0
  for (int i = 0; i < 256; i++) {
    brightnessCurve[i] = pow((i / 256.), 2.0);
  }
}

void JFixtureAddr::writeRGB(int id, float r, float g, float b, char channel,
                            floatColor **leds) {
  if (!leds)
    return;

  if (r < leds[channel][id].r)
    r = leds[channel][id].r;
  if (g < leds[channel][id].g)
    g = leds[channel][id].g;
  if (b < leds[channel][id].b)
    b = leds[channel][id].b;

  if (r < e->rgbaBackground[0])
    r = e->rgbaBackground[0];
  if (g < e->rgbaBackground[1])
    g = e->rgbaBackground[1];
  if (b < e->rgbaBackground[2])
    b = e->rgbaBackground[2];

  leds[channel][id].r = r;
  leds[channel][id].g = g;
  leds[channel][id].b = b;
  // Check values in leds
  // Convert leds[2] to one RGB float pair
  // uint16_t rTest = 0;
  // char splitNew[2] = {ledsTest[0].r, ledsTest[0].g};
  // memcpy(&rTest, splitNew, 2);
  // float rTestF = rTest / 65535.;

  // rgb[0] = pow(r, 2.0) * 65535;
  // rgb[1] = pow(g, 2.0) * 65535;
  // rgb[2] = pow(b, 2.0) * 65535;
  // leds[channel][(id*2)+0] = CRGB(split[1][0], split[1][1], split[0][0]); //
  // Different, because FastLED set to WS2812B instead of NEOPIXEL
  // leds[channel][(id*2)+1] = CRGB(split[0][1], split[2][0], split[2][1]);
}

void JFixtureAddr::writeRGBHard(
    int id, float r, float g, float b, char channel,
    floatColor **leds) { // Doesn't check previous values, but
                         // does not  backgroundColor
  if (!leds)
    return;

  if (r < e->rgbaBackground[0])
    r = e->rgbaBackground[0];
  if (g < e->rgbaBackground[1])
    g = e->rgbaBackground[1];
  if (b < e->rgbaBackground[2])
    b = e->rgbaBackground[2];

  leds[channel][id].r = r;
  leds[channel][id].g = g;
  leds[channel][id].b = b;
  // uint16_t rgb[3];
  // char split[3][2];
  // rgb[0] = r * 65535;
  // rgb[1] = g * 65535;
  // rgb[2] = b * 65535;
  // for (int i = 0; i < 3; i++) {
  //   memcpy(split[i], &rgb[i], 2);
  // }
  // leds[channel][(id * 2) + 0] = CRGB(split[1][0], split[1][1],
  // split[0][1]); leds[channel][(id * 2) + 1] = CRGB(split[2][1],
  // split[0][0], split[2][0]);
}

void JFixtureAddr::testLED() {
  for (int h = 0; h < 3; h++) {
    for (int j = 0; j < numStrings; j++) {
      for (int i = 0; i < numLedsPerString; i++) {
        float c[3] = {0, 0, 0};
        c[h] = 1.0;
        writeRGBHard(i, c[0], c[1], c[2], j, leds);
      }
    }
    writeLeds();
    delay(1000);
  }
}

void JFixtureAddr::setChannel(char i, float v) {
  for (int i = 0; i < numChannels; i++) {
    channels[i] = v;
  }
}

void JFixtureAddr::allBlack(bool bWrite) {
  for (int j = 0; j < numStrings; j++) {
    for (int i = 0; i < numLedsPerString; i++) {
      writeRGBHard(i, 0, 0, 0, j, leds);
    }
  }
  if (bWrite)
    writeLeds();
}

void JFixtureAddr::writeLeds() {
  // for (int j = 0; j < numStrings; j++) {
  // memcpy(ledsToWrite[j], leds[j], numLedsPerString * 2 * sizeof(CRGB));
  // }
#ifndef NEOPIXELBUS
  for (int j = 0; j < numStrings; j++) {
    for (int i = 0; i < numLedsPerString; i++) {
      floatColor *c = &leds[j][i];
      unsigned short rgb[3];
      switch (ledMode) {
      case J_WS2812B: {
        rgb[0] = pow(c->r, 2.0) * 255;
        // rgb[0] = c->r * 65535;
        rgb[1] = pow(c->g, 2.0) * 255;
        rgb[2] = pow(c->b, 2.0) * 255;

        ledsToWrite[j][i] = CRGB(rgb[0], rgb[1], rgb[2]);
      } break;
      case J_WS2816B: {
        // rgb[0] = pow(c->r, 2.0) * 65535;
        // rgb[0] = c->r * 65535;
        // rgb[1] = pow(c->g, 2.0) * 65535;
        // rgb[2] = pow(c->b, 2.0) * 65535;

        rgb[0] = (c->r * c->r * 65535); // Square and scale to 65535
        rgb[1] = (c->g * c->g * 65535);
        rgb[2] = (c->b * c->b * 65535);

        char split[3][2];
        // for (int i = 0; i < 3; i++) {
        // memcpy(split[i], &rgb[i], 2);
        // }

        // Directly assign values to split, no need for memcpy
        split[0][0] = (rgb[0] >> 8) & 0xFF; // High byte
        split[0][1] = rgb[0] & 0xFF;        // Low byte

        split[1][0] = (rgb[1] >> 8) & 0xFF; // High byte
        split[1][1] = rgb[1] & 0xFF;        // Low byte

        split[2][0] = (rgb[2] >> 8) & 0xFF; // High byte
        split[2][1] = rgb[2] & 0xFF;        // Low byte
                                            //
        ledsToWrite[j][(i * 2) + 0] =
            CRGB(split[1][0], split[1][1], split[0][1]);
        ledsToWrite[j][(i * 2) + 1] =
            CRGB(split[2][1], split[0][0], split[2][0]);

      } break;
      }
      // Decode to float before pow...
      // ledsToWrite[j][i].r = pow(ledsToWrite[j][i].r, 2);
      // ledsToWrite[j][i].g = pow(ledsToWrite[j][i].g, 2);
      // ledsToWrite[j][i].b = pow(ledsToWrite[j][i].b, 2);
    }
  }
#endif
#ifdef ADAFRUIT_NEOPIXEL
  for (int j = 0; j < numStrings; j++) {
    for (int i = 0; i < numLedsPerString; i++) {
      pixels.setPixelColor(i, pixels.Color(ledsToWrite[j][i].red,
                                           ledsToWrite[j][i].green,
                                           ledsToWrite[j][i].blue));
    }
    pixels.show();
  }

#elif NEOPIXELBUS
  for (int i = 0; i < NEOPIXELBUS_NUMLEDS; i++) {
    strip.SetPixelColor(i,
                        Rgb48Color(leds[0][i].r * 65535, leds[0][i].g * 65535,
                                   leds[0][i].b * 65535));
  }
  strip.Show();
#else
  FastLED.show();
#endif
}

void JFixtureAddr::update() {
  JFixture::update();
  if (bStatic) {
    for (int j = 0; j < numStrings; j++) {
      for (int i = 0; i < numLedsPerString; i++) {
        writeRGBHard(i, channels[0] * getBrightness(),
                     channels[1] * getBrightness(),
                     channels[2] * getBrightness(), j, leds);
      }
    }
    writeLeds();
    return;
  }
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
    // for (float j = 0; j < numStrings; j++) {
    //   for (float i = 0; i < numLedsPerString; i++) {
    //     float val = ofNoise(i * noiseScale,
    //                         (j * horizontalPixelDistance) * noiseScale,
    //                         millis() * noiseTimeScale);
    //     val = pow(val, 2.0);
    //     if (val < 0.005)
    //       val = 0.005;
    //     writeRGB(i, 0.0, 0.0, val, j, leds);
    //   }
    // }
    // writeLeds();
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
    if (millis() - lastUpdatedLive < 1) { // Delay 1 ms? ...
      return;
    }
    allBlack();
    bool bEventUpdate = false;
    for (char i = 0; i < MAX_EVENTS; i++) {
      if (events[i]) {
        events[i]->update();
        if (events[i]->bActive) {
          bEventUpdate = true;
          events[i]->draw(leds, numLedsPerString, numStrings,
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
    // if (bEventUpdate) {
    //   delayMicroseconds(1500); // Or even 0?
    // } else {
    //   delay(2);
    // }
    lastUpdatedLive = millis();
    // if(bUseCanvas){
    //
    // canvas.waitCompletion();
    // canvasToLeds();
    // }
  } break;
  }
  // delayMicroseconds(500);
}

void JFixtureAddr::blink(char num, short dur, short delayTime, char channel) {
  float values[3] = {0};
  values[channel] = 1.0;
  writeRGB(1, values[0], values[1], values[2], 0, leds);
}
