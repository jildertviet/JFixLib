#pragma once
#include "JFixture.h"
// #include <esp_wifi_internal.h>
#include "esp_private/wifi.h"

#define MAX_MSG 128
#define MAX_MSG_LEN 512

class JEspnowDevice;
extern JEspnowDevice *e;

class JEspnowMsg {
public:
  JEspnowMsg(){};
  uint8_t data[MAX_MSG_LEN];
  short len = 0;
};

class JEspnowDevice : public JFixture {
public:
  JEspnowMsg msgBuffer[MAX_MSG];
  uint8_t replyAddr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  uint8_t myAddr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  unsigned long lastReceived = 0;
  int pingOffsetSeed = 0;
  String networkName = "JV_";
  bool bEspnowEnabled = true;
  bool bEspnowPingEnabled = true;
  void (*receiveMotorCommandsPtr)(const uint8_t *, const uint8_t *, int,
                                  int) = nullptr;

  JEspnowDevice();
  virtual void setup(String networkName) override;
  String randomPw();
  bool checkAddressed(const uint8_t *data);
  static void receive(const uint8_t *mac_addr, const uint8_t *data,
                      int data_len);

  void setBootMode(char *data);
  void saveMsg(const uint8_t *data, const short data_len);
  void parseMultiple(const uint8_t *data, const uint8_t data_len);
  void parseMsgs();
  void initEspnow(String networkName);
  void addPeer(uint8_t *addr);
  void sendPing(bool bOverride = false) override;
};
