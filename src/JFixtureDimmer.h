#pragma once
#include "JEspnowDevice.h"
#include "JFixture.h"
#include "JWS2812B_status.h"

class JFixtureDimmer : public JEspnowDevice, public JWS2812B_status {
public:
  enum PwmMode { PWM12BIT };
  PwmMode pwmMode = PwmMode::PWM12BIT;
  uint8_t *pins;
  float brightnessCurve[256];

  JFixtureDimmer(){};

  void setup(char numChannels = 4, uint8_t *pins = nullptr) {
    channels = new float[numChannels];
    this->pins = new uint8_t[numChannels];
    this->numChannels = numChannels;
    if (pins) {
      memcpy(this->pins, pins, numChannels);
      for (int i = 0; i < numChannels; i++) {
        ledcAttachPin(pins[i], 1 + i);
        channels[i] = 0;
        switch (pwmMode) {
        case PWM12BIT:
          ledcSetup(i + 1, 9000, 12); // Hz / bitdepth
          ledcWrite(i + 1, 0);        // Turn all off
          break;
        }
      }
    }
    initCurve();
    testLED();
  }

  void initCurve() { // 0.0 -- 1.0
    for (int i = 0; i < 256; i++) {
      if (i < 30) {
        float v = i / 30.0;
        v = pow(v, 0.5);
        v *= 30.0;
        brightnessCurve[i] = pow((v / 256.), 2.0);
      } else {
        brightnessCurve[i] = pow((i / 256.), 2.0);
      }
    }
  }

  void setLED(char channel, int value) { // Receives 0 - 255
    float v = brightnessCurve[value];
    if (pwmMode == PWM12BIT) {
      value = v * 4096;
    } else {
      value = v * 256;
    }
    ledcWrite(channel + 1, value);
  }
  void testLED() {
    for (int i = 0; i < numChannels; i++) {
      setLED(i, 100);
      delay(700);
      setLED(i, 0);
    }
  };
  virtual void update() override {
    JFixture::update();
    show();
  }

  void showSucces() override {}
  void show() override {
    for (int i = 0; i < numChannels; i++) {
      setLED(i, channels[i] * getBrightness() * 255);
    }
  };
  void setChannel(char i, float val) override {
    if (i < numChannels) {
      channels[i] = val;
    }
  }

  void blink(char num = 1, short dur = 100, short delayTime = 100,
             char channel = 0) override {
    for (int i = 0; i < num; i++) {
      setLED(0, 200);
      delay(dur);
      setLED(0, 0);
      delay(delayTime);
    }
  }
};
