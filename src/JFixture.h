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
#define NUM_PARAMETER_BUSSES 64
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
    for (int i = 0; i < NUM_PARAMETER_BUSSES; i++) {
      parameterBusses[i] = 0.5;
    }
    // channels = new float[numChannels]; // Do in specific classes
  }
  int id = -1;
  float busses[NUM_BUSSES]; // What is this used for...
  float parameterBusses[NUM_PARAMETER_BUSSES];
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
  virtual void linkBus(const uint8_t *data, int data_len, float *busses){};
  virtual void setCustomArg(const uint8_t *data, int data_len){};
  virtual void sync(int eventID){};
  virtual void deleteEvents(){};
  float rgbaBackground[4] = {0.0};
  // bool bDraw = false;
  bool bAliveBlink = true;
  bool bStatic = false;

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
    if (bAliveBlink == false)
      return;
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
      if (EEPROM.read(1) == 1) {
        Serial.println("Start as static light");
        bStatic = true;
        float rgba[4];
        for (int i = 0; i < 4; i++) {
          EEPROM.get(2 + (i * sizeof(float)), rgba[i]);
          Serial.println(rgba[i]);
        }
        rgbaBackground[0] = rgba[0];
        rgbaBackground[1] = rgba[1];
        rgbaBackground[2] = rgba[2];
        brightness = rgba[3];
        Serial.println();
      }
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

  void setParameterBus(const uint8_t *data, int data_len) {
    char busIndex = 0;
    float value = 0.0;
    memcpy(&busIndex, data, sizeof(char));
    memcpy(&value, data + sizeof(char), sizeof(float));
    if (busIndex < NUM_PARAMETER_BUSSES) {
      parameterBusses[busIndex] = value;
    }
  }

  void setParameterBusN(const uint8_t *data, int data_len) {
    char busIndex = 0; // StartIndex
    memcpy(&busIndex, data, sizeof(char));
    char numValues = (data_len - sizeof(char)) / sizeof(float);

    // Serial.println("Set bus N, numValues: ");
    // Serial.println((int)numValues);
    if (busIndex < NUM_PARAMETER_BUSSES &&
        (busIndex + numValues - 1) < NUM_PARAMETER_BUSSES) {
      for (char i = 0; i < numValues; i++) {
        memcpy(parameterBusses +
                   (busIndex + i), // (int)(busIndex), removed (int))
               data + sizeof(char) + (sizeof(int) * i),
               sizeof(float)); // 4, start of values array
      }
    }
  }

private:
  float brightness = 0.1; // Use lagger[0]
};
