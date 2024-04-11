#pragma once
#include "JFixture.h"
// #include <esp_wifi_internal.h>
#include "esp_private/wifi.h"

#define MAX_MSG 20
#define MAX_MSG_LEN 250

class JEspnowDevice;
JEspnowDevice *e = nullptr;

class JEspnowMsg {
public:
  JEspnowMsg(){};
  uint8_t data[MAX_MSG_LEN];
  uint8_t len = 0;
};

class JEspnowDevice : public JFixture {
public:
  JEspnowMsg msgBuffer[MAX_MSG];
  uint8_t replyAddr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  uint8_t myAddr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  unsigned long lastReceived = 0;
  int pingOffsetSeed = 0;
  String networkName = "JV_";

  virtual void setup(String networkName) override {
    JFixture::setup(networkName);
    initEspnow("JV_");
    pingOffsetSeed = random(2000);
  }
  String randomPw() {
    String r = "01234567";
    for (int i = 0; i < 8; i++) {
      r[i] = random(255);
    }
    return r;
  }

  JEspnowDevice() {
    // initEspnow("JV_");
  }

  bool checkAddressed(const uint8_t *data) {
    bool bAddressed = true;
    bool bBroadcast = true;
    for (int i = 0; i < 6; i++) {
      if (bBroadcast && data[i + 1] != 0xFF) {
        bBroadcast = false;
      }
    }
    if (bBroadcast)
      return true;

    for (int i = 0; i < 6; i++) { // Check if this msg is for this ESP32
      if (myAddr[i] != data[i + 1]) {
        bAddressed = false;
      }
    }
    return bAddressed;
  }

  static void receive(const uint8_t *mac_addr, const uint8_t *data,
                      int data_len) {
    char msgType = data[0];
    // Serial.println("ESPNOW msg received");
    if (msgType != 'a') { // Ignore 'alive'-packets from other ESP32's
      e->lastReceived = millis();
    }

    switch (msgType) {
    case 0x10: { // [msgType, address[6], minutesToSleep[2]]
      if (e->checkAddressed(data)) {
        e->oneShots[JModes::OneShot::START_DEEPSLEEP] = true;
        // Read first argument as float ...
        memcpy(&e->oneShots[JModes::OneShot::START_DEEPSLEEP].arguments[0],
               data + 6 + 1, sizeof(float));
        // blink(Channel::RED, 1); // Blink in parse part?
      }
    } break;
    case 0x11: { // {0x11, a, d, d, r, e, s, 0x05} : set ESP32 with address
                 // 'addres' to ID 0x05
      if (e->checkAddressed(data)) {
        Serial.println("Set ID");
        e->id = data[7];
        EEPROM.write(0, e->id);
        EEPROM.commit();
        Serial.println((int)e->id);
      }
    } break;
    case 0x15: {
      Serial.print("0x15 Ota Server");
      if (e->checkAddressed(data)) {
        e->parseOtaServerVariables(data, data_len);
        e->otaMode = Mode::START_OTA_SERVER;
      }
    } break;
    case 0x20: { // SET_RGBW
      if (e->checkAddressed(data)) {
        // int numChannels = (data_len - 6 - 1) / 4;
        e->oneShots[JModes::OneShot::SET_CHANNELS] = true;
        memcpy(&e->oneShots[JModes::OneShot::SET_CHANNELS].arguments[0],
               data + 6 + 1, data_len - 6 - 1);
      }
    } break;
    case 0x21: {
      if (e->checkAddressed(data)) {
        e->oneShots[JModes::OneShot::SET_BRIGHTNESS] = true;
        memcpy(&e->oneShots[JModes::OneShot::SET_BRIGHTNESS].arguments[0],
               data + 6 + 1, sizeof(float) * 1);
      }
    } break;
    case 0x22: {
      if (e->checkAddressed(data)) {
        char lagID = *(data + 6 + 1);
        switch (lagID) {
        case 0: {
          memcpy(&(e->brightnessLag->lagTime), data + 6 + 1 + 1, sizeof(float));
        } break;
        }
      }
    }
    case 0x23:   // addEvent
    case 0x24: { // addEnv
      if (e->checkAddressed(data)) {
        e->saveMsg(data, data_len);
        // e->addEnv(data+6+1, data_len - 6 - 1);
      }
    } break;
    case 0x25: {
      if (e->checkAddressed(data)) {
        e->deleteEvents();
      }
    } break;
    case 0x26: {
      if (e->checkAddressed(data)) {
        memcpy(e->rgbaBackground, data + 6 + 1, sizeof(float) * 4);
        // bDraw = true;
      }
    } break;
    case 0x27: {
      if (e->checkAddressed(data)) {
        memcpy(e->viewportOffset, data + 6 + 1, sizeof(float) * 2);
      }
    }
    case 0x28:   // setVal
    case 0x29:   // setValN
    case 0x30:   // multiple / grouped
    case 0x31: { // customArg
      if (e->checkAddressed(data)) {
        e->saveMsg(data, data_len);
      }
    } break;
    case 0x32: {
      if (e->checkAddressed(data)) {
        int eventID;
        memcpy(&eventID, data + 6 + 1, sizeof(int));
        e->sync(eventID);
      }
    }
    case 0x33: { // Set RGBW 2-bit short
      unsigned short values[4];
      memcpy(&values, data + 1 + (e->id * (4 * sizeof(short))),
             4 * sizeof(short));
      for (int i = 0; i < 4; i++) {
        e->setChannel(i, values[i] / 65536.);
      }
    } break;
    case 0x34: { // Set brightness 4-bit float
      float b;
      memcpy(&b, data + 1 + (e->id * sizeof(float)), sizeof(float));
      e->setBrightness(b);
    } break;
    default:
      break;
    }
  }

  void saveMsg(const uint8_t *data, const uint8_t data_len) {
    // for (int i = 0; i < data_len; i++) {
    // Serial.print((int)data[i]);
    // Serial.print(" ");
    // }
    for (int i = 0; i < MAX_MSG; i++) {
      if (e->msgBuffer[i].len == 0) {
        // Serial.print("Save @ ");
        // Serial.println(i);
        memcpy(e->msgBuffer[i].data, data, data_len);
        e->msgBuffer[i].len = data_len;
        return;
      }
    }
  }

  void parseMultiple(const uint8_t *data, const uint8_t data_len) {
    char numMsg = data[0];
    unsigned char lengths[numMsg];
    unsigned char writePos = numMsg + 1;
    memcpy(&lengths, data + 1, numMsg);
    // Serial.print("Num msg in grouped msg: ");
    // Serial.println((int)numMsg);
    for (int i = 0; i < numMsg; i++) {
      // Serial.print("Length: ");
      // Serial.println(lengths[i]);
      saveMsg(data + writePos, lengths[i]);
      writePos += lengths[i];
    }
    parseMsgs();
  }

  void parseMsgs() {
    for (int i = 0; i < MAX_MSG; i++) { // Reverse order
      if (msgBuffer[i].len) {
        switch (msgBuffer[i].data[0]) {
        case 0x23:
          addEvent((msgBuffer[i].data) + 6 + 1, msgBuffer[i].len - 6 - 1);
          break;
        case 0x24:
          addEnv((msgBuffer[i].data) + 6 + 1, msgBuffer[i].len - 6 - 1);
          break;
        case 0x28:
          setVal((msgBuffer[i].data) + 6 + 1, msgBuffer[i].len - 6 - 1);
          break;
        case 0x29:
          setValN((msgBuffer[i].data) + 6 + 1, msgBuffer[i].len - 6 - 1);
          break;
        case 0x30:
          msgBuffer[i].len = 0;
          parseMultiple((msgBuffer[i].data) + 6 + 1, msgBuffer[i].len - 6 - 1);
          break;
        case 0x31:
          setCustomArg((msgBuffer[i].data) + 6 + 1, msgBuffer[i].len - 6 - 1);
          break;
        }
        msgBuffer[i].len = 0;
      }
    }
  }

  void initEspnow(String networkName) {
    this->networkName = networkName;
    WiFi.softAP(networkName.c_str(), randomPw().c_str(), CHANNEL,
                true); // This sets the channel... ?
    Serial.print("Using WiFi channel: ");
    Serial.println(CHANNEL);
    Serial.print("AP MAC address: ");
    Serial.println(WiFi.softAPmacAddress());
    String addr = WiFi.softAPmacAddress(); // 0D:XD:33: etc, replace : with
                                           // 0x and prepend line w/ 0x
    addr.replace(":", ",0x");
    addr = "0x" + addr;
    Serial.println(addr); // For copying in SC array, or JSON
    WiFi.softAPmacAddress(myAddr);
    WiFi.disconnect();

    //==================================================
    WiFi.mode(WIFI_STA);

    /*Stop wifi to change config parameters*/
    esp_wifi_stop();
    esp_wifi_deinit();

    /*Disabling AMPDU is necessary to set a fix rate*/
    wifi_init_config_t my_config =
        WIFI_INIT_CONFIG_DEFAULT(); // We use the default config ...
    my_config.ampdu_tx_enable = 0;  //... and modify only what we want.
    esp_wifi_init(&my_config);      // set the new config

    esp_wifi_start();

    /*You'll see that in the next subsection ;)*/
    esp_wifi_set_channel(CHANNEL, WIFI_SECOND_CHAN_NONE);

    /*set the rate*/
    esp_wifi_internal_set_fix_rate(WIFI_IF_STA, true, DATARATE);

    //==================================================

    if (esp_now_init() == ESP_OK) {
      Serial.println("ESPNow Init Success");
    } else {
      Serial.println("ESPNow Init Failed, restart...");
      ESP.restart();
    }
    e = this;
    esp_now_register_recv_cb(receive);
  }

  void addPeer(uint8_t *addr) {
    esp_now_peer_info_t slave;

    slave.channel = CHANNEL;
    slave.encrypt = 0;
    slave.ifidx = WIFI_IF_AP;
    memcpy(slave.peer_addr, addr, 6);
    if (slave.channel == CHANNEL) {
      Serial.print("Slave Status: ");
      const esp_now_peer_info_t *peer = &slave;
      const uint8_t *peer_addr = slave.peer_addr;
      bool exists = esp_now_is_peer_exist(peer_addr);
      if (exists) {
        Serial.println("Already Paired");
      } else {
        // Slave not paired, attempt pair
        esp_err_t addStatus = esp_now_add_peer(peer);
        if (addStatus == ESP_OK) {
          // Pair success
          Serial.println("Pair success");
        } else if (addStatus == ESP_ERR_ESPNOW_NOT_INIT) {
          Serial.println("ESPNOW Not Init");
        } else if (addStatus == ESP_ERR_ESPNOW_ARG) {
          Serial.println("Invalid Argument");
        } else if (addStatus == ESP_ERR_ESPNOW_FULL) {
          Serial.println("Peer list full");
        } else if (addStatus == ESP_ERR_ESPNOW_NO_MEM) {
          Serial.println("Out of memory");
        } else if (addStatus == ESP_ERR_ESPNOW_EXIST) {
          Serial.println("Peer Exists");
        } else {
          Serial.println("Not sure what happened");
        }
      }
    } else {
      // No slave found to process
      Serial.println("No Slave found to process");
    }
  }

  void sendPing(bool bOverride = false) override {
    if (otaMode != IDLE)
      return;

    // Only send when no msg is received for x seconds
    if (millis() > lastReceived + (60000 + pingOffsetSeed) && millis() > 100 ||
        bOverride) {
      WiFi.mode(WIFI_OFF);
      WiFi.mode(WIFI_STA);

      initEspnow(networkName);

      // if (esp_now_init() == ESP_OK) {
      //   Serial.println("ESPNow Init Success");
      // } else {
      //   Serial.println("ESPNow Init Failed, restart...");
      //   ESP.restart();
      // }
      //
      delay(100);
      uint8_t broadcastAddr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
      memcpy(replyAddr, &broadcastAddr, 6);
      addPeer(replyAddr);

      uint8_t msg[7] = {'a', 'l', 'i', 'v', 'e', 0, 0};
      // The function below will add delay() !
      int v = measureBattery(); // This will return 0 if no batteryPin is set
      // int v = 1;
      memcpy(msg + 1, &v, 4); // Prefix is 'a'
      memcpy(
          msg + 1 + 4, &VERSION,
          2); // Msg looks like: 'a', batteryVal (4 bytes), version (2 bytes);

      esp_now_send(replyAddr, msg, 7);
      Serial.println("Send ping!");
      delay(100);
      WiFi.mode(WIFI_OFF);
      initEspnow(networkName);
      lastReceived = millis();
    }
  }
};
