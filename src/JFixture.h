#pragma once

#include "defines.h"
#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "EEPROM.h"
#include "JModes.h"
#include "JOtaServer.h"
#include "JEspnowDevice.h"

using namespace std;
#define NUM_BUSSES  12

class JFixture: public JOtaServer, public JEspnowDevice{
  public:
  JFixture(){
    memset(busses, 0x00, NUM_BUSSES * sizeof(float));
  }
  float busses[NUM_BUSSES];

  void setup(String networkName = "___"){
    Serial.begin(115200);
    Serial.println("Start");
    initEspnow(networkName);  
  }

  virtual void update(){
    handleOtaServer();
    checkOneShots();
  }

  void checkOneShots(){
    std::map<OneShot, JMsgArguments>::iterator it;
    for (it=oneShots.begin(); it!=oneShots.end(); it++){
      if(it->second.bActive == true){
        Serial.println("OneShot triggered");
        switch(it->first){
        case OneShot::START_DEEPSLEEP:{
              // How to get the duration? w/ Busses?
          float* minutesToSleep = &busses[0]; // For readability
          if(busses[0] > 0){ // busses[0] = minutes
        // esp_sleep_enable_ext0_wakeup(GPIO_NUM_14,1);
            esp_sleep_enable_timer_wakeup(*minutesToSleep * 60 * 1000000ULL);
            esp_deep_sleep_start();
          }
        }
        break;
        }
      }
    }
  }
};
