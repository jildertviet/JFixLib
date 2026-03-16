#ifndef DIMMER_H
#define DIMMER_H

#include "jfix_platform.h"
#ifndef JFIX_EMULATION
#include "driver/ledc.h"
#include "esp_err.h"
#endif
#include <vector>
#include <cstdint>

class Dimmer {
public:
  Dimmer();
  esp_err_t init(const std::vector<int> &pins);
  void setChannel(int channel, float value); // value 0.0 to 1.0
  void setBrightness(float brightness);      // global brightness 0.0 to 1.0
  void show();
  void test();
  void blink(uint8_t num = 1, uint16_t dur = 100, uint16_t delayTime = 100,
             uint8_t channel = 0);

private:
  std::vector<int> _pins;
  std::vector<float> _channelValues;
  float _globalBrightness = 1.0f;

#ifndef JFIX_EMULATION
  static const ledc_mode_t MODE = LEDC_LOW_SPEED_MODE;
  static const ledc_timer_t TIMER = LEDC_TIMER_0;
  static const ledc_timer_bit_t RESOLUTION = LEDC_TIMER_12_BIT;
  static const uint32_t FREQUENCY = 9000;
#endif
};

extern Dimmer dimmer;

#endif // DIMMER_H
