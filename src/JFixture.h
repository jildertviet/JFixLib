#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "EEPROM.h"
#include "JOtaServer.h"

class JFixture: public JOtaServer{
  public:
  JFixture(){

  }

};
