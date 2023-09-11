#pragma once
#include "defines.h"
#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "EEPROM.h"
#include "JOtaServer.h"
#include "JEspnowDevice.h"

class JFixture: public JOtaServer, public JEspnowDevice{
  public:

  JFixture(){
  
  }

  
  void setup(String networkName = "___"){
    Serial.begin(115200);
    Serial.println("Start");
    initEspnow(networkName);  
  }
};
