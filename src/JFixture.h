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
#define NUM_PARAMETER_BUSSES 128
#define NUM_LAGGERS 4

enum Channel { RED, GREEN, BLUE, WHITE };

class JFixture : public JOtaServer, public JModes {
public:
  JLag laggers[NUM_LAGGERS];
  JLag *brightnessLag;
  JFixture();
  int id = -1;
  float busses[NUM_BUSSES] = {0}; // What is this used for...
  float parameterBusses[NUM_PARAMETER_BUSSES] = {0.5};
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

  virtual void setup(String networkName = "___");

  virtual void update();
  virtual void sendPing(bool b = false){};
  virtual void blink(uint8_t num = 1, short dur = 100, short delayTime = 100,
                     uint8_t channel = 0){};

  void setLedBuiltin(char pin = 5);

  void aliveBlink(bool bOverride = false);

  static void digitalWriteBuiltinLed(char pin, char status);

  void checkOneShots();
  void readEEPROM();
  // virtual void setRGBW(float r, float g, float b, float w, bool bShow =
  // true){};
  virtual void setChannel(uint8_t i, float v){};
  virtual void setBrightness(float b);
  virtual void show(){};
  void updateLaggers();
  float getBrightness();

  void setParameterBus(const uint8_t *data, int data_len);
  void setParameterBusN(const uint8_t *data, int data_len);

private:
  float brightness = 0.1; // Use lagger[0]
};
