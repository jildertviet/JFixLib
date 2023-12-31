#pragma once
#include <map>

#define NUM_ONESHOT_FLAGS 1
#define NUM_ARGUMENTS 16

class JMsgArguments{
  public:
    JMsgArguments(){};
    bool bActive = false;
    float arguments[NUM_ARGUMENTS];
    JMsgArguments& operator=(bool isActive) {
        bActive = isActive;
        return *this;
    }
};

class JModes{
  public:
  JModes(){
    // oneShots[OneShot::START_DEEPSLEEP] = false;
  };
  enum OneShot{
    START_DEEPSLEEP,
    SET_CHANNELS,
    SET_BRIGHTNESS
    // START_OTA_SERVER
  };
  std::map<OneShot, JMsgArguments> oneShots{
    {START_DEEPSLEEP, JMsgArguments()},
    {SET_CHANNELS, JMsgArguments()},
    {SET_BRIGHTNESS, JMsgArguments()}
    // {START_OTA_SERVER, JMsgArguments()}
  };
};
