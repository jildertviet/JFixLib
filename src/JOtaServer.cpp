#include "JOtaServer.h"

void HttpEvent(HttpEvent_t *event) {
  switch (event->event_id) {
  case HTTP_EVENT_ERROR:
    Serial.println("Http Event Error");
    break;
  case HTTP_EVENT_ON_CONNECTED:
    Serial.println("Http Event On Connected");
    break;
  case HTTP_EVENT_HEADER_SENT:
    Serial.println("Http Event Header Sent");
    break;
  case HTTP_EVENT_ON_HEADER:
    Serial.printf("Http Event On Header, key=%s, value=%s\n", event->header_key,
                  event->header_value);
    break;
  case HTTP_EVENT_ON_DATA:
    break;
  case HTTP_EVENT_ON_FINISH:
    Serial.println("Http Event On Finish");
    break;
  case HTTP_EVENT_DISCONNECTED:
    Serial.println("Http Event Disconnected");
    break;
  }
}

bool JOtaServer::checkOtaServer() {
  delay(1000);
  otastatus = HttpsOTA.status();
  if (otastatus == HTTPS_OTA_SUCCESS) {
    Serial.println("Firmware written successfully. To reboot device, call "
                   "API ESP.restart() or PUSH restart button on device");
    // ESP.restart();
    return true;
  } else if (otastatus == HTTPS_OTA_FAIL) {
    Serial.print("Firmware Upgrade Fail ");
    Serial.println(numTries++);
    if (numTries > 30) {
      // ESP.restart();
    }
    return false;
  }
  return false; // ...
}

bool JOtaServer::parseOtaServerVariables(const uint8_t *data, int data_len) {
  Serial.println(" addressed");
  DynamicJsonDocument doc(200); // {"ssid":"xxx"}
  char *json = new char[data_len - 7];
  memcpy(json, data + 1 + 6, data_len - 7); // ID, mac[6], JSONMSG
  DeserializationError error = deserializeJson(doc, json);
  String ssid_ = doc["ssid"];
  ssid = new char[ssid_.length() + 1];
  ssid_.toCharArray(ssid, ssid_.length() + 1);
  String password_ = doc["password"];
  password = new char[password_.length() + 1];
  password_.toCharArray(password, password_.length() + 1);
  String url_ = doc["url"];
  url = new char[url_.length() + 1];
  url_.toCharArray(url, url_.length() + 1);
  // memcpy(url, &url_, url_.length());
  Serial.println(ssid);
  Serial.println(password);
  Serial.println(url);

  return true;
}
bool JOtaServer::startOtaServer() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int attemptCount = 0;
  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
    attemptCount++;
    if (attemptCount >= 10) {
      Serial.println(" Connection failed");
      return false;
    }
  }

  Serial.print("Connected to ");
  Serial.println(ssid);

  HttpsOTA.onHttpEvent(HttpEvent);
  Serial.println("Starting OTA");
  HttpsOTA.begin(url, server_certificate);

  Serial.println("Please Wait it takes some time ...");
  return true;
}
void JOtaServer::showSucces() {}

void JOtaServer::handleOtaServer() {
  switch (otaMode) {
  case JOtaServer::START_OTA_SERVER: {
    if (startOtaServer()) { // Will return false if WiFi fails to connect
      // blinkLed(1, 250, 3);
      // setLED(1, 50);
      showSucces();
    } else {
      ESP.restart();
    }
    otaMode = HANDLE_OTA_SERVER;
  } break;
  case JOtaServer::HANDLE_OTA_SERVER: {
    otastatus = HttpsOTA.status();
    if (otastatus == HTTPS_OTA_SUCCESS) {
      Serial.println("Firmware written successfully. To reboot device, call "
                     "API ESP.restart() or PUSH restart button on device");
      ESP.restart();
    } else if (otastatus == HTTPS_OTA_FAIL) {
      Serial.println("Firmware Upgrade Fail");
      ESP.restart();
    }
    delay(1000);
  } break;
  }
}
