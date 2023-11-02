#pragma once
#include "JFixture.h"

class JEspnowDevice;
JEspnowDevice* e = nullptr;

class JEspnowDevice: public JFixture{
  public:
    uint8_t replyAddr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t myAddr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    unsigned long lastReceived = 0;
    
    virtual void setup(String networkName) override{
      JFixture::setup(networkName);
      initEspnow("JV_");
    }
    String randomPw(){
      String r = "01234567";
      for(int i=0; i<8; i++){
        r[i] = random(255);
      }
      return r;
    }
    
   
    JEspnowDevice(){
      // initEspnow("JV_");
    }
    
    bool checkAddressed(const uint8_t* data){
      bool bAddressed = true;
      for(int i=0; i<6; i++){ // Check if this msg is for this ESP32
        if(myAddr[i] != data[i+1]){
            bAddressed = false;
        }
      }
      return bAddressed;
    }
    virtual void addEvent(const uint8_t *data, int data_len){};
    virtual void addEnv(const uint8_t *data, int data_len){};

    static void receive(const uint8_t *mac_addr, const uint8_t *data, int data_len){
      char msgType = data[0];
      if(msgType != 'a'){
        e->lastReceived = millis();
      }

      switch(msgType){
      case 0x10:{ // [msgType, address[6], minutesToSleep[2]]
        if(e->checkAddressed(data)){
          e->oneShots[JModes::OneShot::START_DEEPSLEEP] = true;
          // Read first argument as float ... 
          memcpy(&e->oneShots[JModes::OneShot::START_DEEPSLEEP].arguments[0], data+6+1, sizeof(float));
          // blink(Channel::RED, 1); // Blink in parse part?
        }
      }
      break;
      case 0x11:{ // {0x11, a, d, d, r, e, s, 0x05} : set ESP32 with address 'addres' to ID 0x05
        if(e->checkAddressed(data)){
          Serial.println("Set ID");
          e->id = data[7];
          EEPROM.write(0, e->id);
          EEPROM.commit();
          Serial.println((int)e->id);
        }
      }
      break;
      case 0x15:{
        Serial.print("0x15 Ota Server");
        if(e->checkAddressed(data)){
          e->parseOtaServerVariables(data, data_len);
          e->otaMode = Mode::START_OTA_SERVER;
        }
      }
      break;
      case 0x20:{ // SET_RGBW
        if(e->checkAddressed(data)){
          // int numChannels = (data_len - 6 - 1) / 4;
          e->oneShots[JModes::OneShot::SET_CHANNELS] = true;
          memcpy(&e->oneShots[JModes::OneShot::SET_CHANNELS].arguments[0], data+6+1, data_len-6-1);
        }
      }
      break;
      case 0x21:{
        if(e->checkAddressed(data)){
          e->oneShots[JModes::OneShot::SET_BRIGHTNESS] = true;
          memcpy(&e->oneShots[JModes::OneShot::SET_BRIGHTNESS].arguments[0], data+6+1, sizeof(float) * 1);
        }
      }
      break;
      case 0x22:{
        if(e->checkAddressed(data)){
          char lagID = *(data+6+1);
          switch(lagID){
            case 0:{
              memcpy(&(e->brightnessLag->lagTime), data+6+1+1, sizeof(float));
            }
            break;
          }
        }
      }
      case 0x23:{
        if(e->checkAddressed(data)){
          e->addEvent(data + 6 + 1, data_len - 6 - 1); // Don't pass address + msgType (0x23)
        }
      }
      break;
      case 0x24:{
        if(e->checkAddressed(data)){
          e->addEnv(data+6+1, data_len - 6 - 1);
        }
      }
      break;
        default:
          break;
      }
    }

    void initEspnow(String networkName){
      WiFi.softAP(networkName.c_str(), randomPw().c_str(), CHANNEL, true); // This sets the channel... ?
      Serial.print("AP MAC address: "); Serial.println(WiFi.softAPmacAddress());
      String addr = WiFi.softAPmacAddress(); // 0D:XD:33: etc, replace : with 0x and prepend line w/ 0x
      addr.replace(":", ",0x");
      addr = "0x" + addr;
      Serial.println(addr); // For copying in SC array, or JSON
      WiFi.softAPmacAddress(myAddr);
      WiFi.disconnect();

      if (esp_now_init() == ESP_OK) {
        Serial.println("ESPNow Init Success");
      } else {
        Serial.println("ESPNow Init Failed, restart...");
        ESP.restart();
      }
      e = this;
      esp_now_register_recv_cb(receive);
    }
}; 
