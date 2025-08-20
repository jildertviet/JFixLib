#pragma once

#include <Arduino.h>
//
#include "HttpsOTAUpdate.h"
#include "WiFi.h"
#include "certificate.h" // Create one, like: static const char *server_certificate = "-----BEGIN CERTIFICATE
#include <ArduinoJson.h>

void HttpEvent(HttpEvent_t *event);

class JOtaServer {
public:
  enum Mode { IDLE, START_OTA_SERVER, HANDLE_OTA_SERVER };
  Mode otaMode = IDLE;
  char *ssid;
  char *password;
  char *url; // Set from MSG

  HttpsOTAStatus_t otastatus;

  int numTries = 0;
  bool checkOtaServer();

  bool parseOtaServerVariables(const uint8_t *data, int data_len);

  bool startOtaServer();
  virtual void showSucces();
  // Implement this in each fixture

  void handleOtaServer();

  // virtual void checkOneShots(){
  //   std::map<OneShot, JMsgArguments>::iterator it;
  //   for (it=oneShots.begin(); it!=oneShots.end(); it++){
  //     if(it->second.bActive == true){
  //       Serial.println("OneShot triggered");
  //       switch(it->first){
  //       case OneShot::START_OTA_SERVER:{
  //           blink(Channel::GREEN, 2); // Do one blink
  //
  //         }
  //       }
  //       break;
  //       }
  //     }
  //   }
  // }
};
