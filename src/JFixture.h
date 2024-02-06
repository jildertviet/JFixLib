#pragma once

#include "defines.h"
//
#include <Arduino.h>
//
#include <esp_now.h>
//
#include <WiFi.h>
//
#include <ESPmDNS.h>
//
#include <WiFiUdp.h>
//
#include <ArduinoOTA.h>
//
#include "EEPROM.h"
//
#include "JModes.h"
//
#include "JOtaServer.h"
//
#include "JLag.h"
// #include "JEspnowDevice.h"

// char jFixVersion[2] = {1, 2};

using namespace std;
#define NUM_BUSSES 12
#define NUM_LAGGERS 4

enum Channel { RED, GREEN, BLUE, WHITE };

class JFixture : public JOtaServer, public JModes {
public:
  JLag laggers[NUM_LAGGERS];
  JLag *brightnessLag;
  JFixture() {
    memset(busses, 0x00, NUM_BUSSES * sizeof(float));
    laggers[0].link(&brightness);
    brightnessLag = &laggers[0];
    // channels = new float[numChannels]; // Do in specific classes
  }
  int id = -1;
  float busses[NUM_BUSSES];
  float *channels = nullptr; // Colors
  char numChannels = 4;
  virtual int measureBattery() { return 0; };
  int batteryPin = -1;

  int ledBuiltin = 5;
  void (*writeStatusLedPtr)(char, char);
  unsigned long lastBlinked = 0;
  int blinkInterval[2] = {30, 1500};
  uint8_t blinkIndex = 0;

  float viewport[2] = {20, 144};
  float viewportOffset[2] = {0, 0}; // This is a scaling factor

  virtual void addEvent(const uint8_t *data, int data_len){};
  virtual void addEnv(const uint8_t *data, int data_len){};
  virtual void setVal(const uint8_t *data, int data_len){};
  virtual void setValN(const uint8_t *data, int data_len){};
  virtual void setCustomArg(const uint8_t *data, int data_len){};
  virtual void sync(int eventID){};
  virtual void deleteEvents(){};
  float rgbaBackground[4] = {0.0};
  // bool bDraw = false;

  // float rgbw[4] = {1.0, 1.0, 1.0, 1.0};

  virtual void setup(String networkName = "___") {
    Serial.begin(115200);
    Serial.println("Start");
    Serial.print("Version: ");
    Serial.print((int)VERSION[0]);
    Serial.print(".");
    Serial.println((int)VERSION[1]);
    readEEPROM();
    writeStatusLedPtr = &digitalWriteBuiltinLed;
  }

  virtual void update() {
    updateLaggers();
    handleOtaServer();
    checkOneShots();
    aliveBlink();
    sendPing();
  }
  virtual void sendPing(bool b = false){};
  virtual void blink(char num = 1, short dur = 100, short delayTime = 100,
                     char channel = 0){};

  void setLedBuiltin(char pin = 5) {
    ledBuiltin = pin;
    pinMode(pin, OUTPUT);
  }

  void aliveBlink(bool bOverride = false) { // GPIO_5
    if (millis() > lastBlinked + blinkInterval[blinkIndex] || bOverride) {
      lastBlinked = millis();
      // digitalWriteBuiltinLed(ledBuiltin, blinkIndex);
      writeStatusLedPtr(ledBuiltin, blinkIndex);
      blinkIndex = (blinkIndex + 1) % 2; // 0, 1, 0, etc
    }
  }

  static void digitalWriteBuiltinLed(char pin, char status) {
    // digitalWriteBuiltinLedAddr(pin, status);
    digitalWrite(pin, status);
  }

  void checkOneShots() {
    // JOtaServer::checkOneShots();
    std::map<OneShot, JMsgArguments>::iterator it;
    for (it = oneShots.begin(); it != oneShots.end(); it++) {
      if (it->second.bActive == true) {
        Serial.println("OneShot triggered");
        switch (it->first) {
        case OneShot::START_DEEPSLEEP: {
          // How to get the duration? w/ Busses?
          Serial.println((float)it->second.arguments[0]);
          float *minutesToSleep = &(it->second.arguments[0]); // For readability
          if (*minutesToSleep > 0) { // busses[0] = minutes
            blink(Channel::RED, 1);  // Do one blink
            // esp_sleep_enable_ext0_wakeup(GPIO_NUM_14,1);
            Serial.println("Go to sleep");
            esp_sleep_enable_timer_wakeup(*minutesToSleep * 60 * 1000000ULL);
            esp_deep_sleep_start();
          }
        } break;
        case OneShot::SET_CHANNELS: {
          for (int i = 0; i < numChannels; i++) {
            setChannel(i, it->second.arguments[i]);
          }
        } break;
        case OneShot::SET_BRIGHTNESS: {
          this->setBrightness(it->second.arguments[0]);
        } break;
        }
        it->second.bActive = false;
      }
    }
  }
  void readEEPROM() {
    if (EEPROM.begin(64)) {
      id = EEPROM.read(0);
      Serial.print("My ID: ");
      Serial.println((int)id);
    } else {
      Serial.println("failed to initialise EEPROM, id not read. Default to 0");
    }
  }
  // virtual void setRGBW(float r, float g, float b, float w, bool bShow =
  // true){};
  virtual void setChannel(char i, float v){};
  virtual void setBrightness(float b) { brightnessLag->set(b); };
  virtual void show(){};
  void updateLaggers() {
    for (int i = 0; i < NUM_LAGGERS; i++) {
      laggers[i].update();
    }
  }
  float getBrightness() { return brightness; };

private:
  float brightness = 0.1; // Use lagger[0]
};
