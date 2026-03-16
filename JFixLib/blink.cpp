#include "blink.h"
#include "jfix_platform.h"

Blink blink;

Blink::Blink() {}

#ifdef JFIX_EMULATION

void Blink::init() {}
void Blink::updateTask(void *pvParameters) {}
void Blink::setBrightness(float b) { _brightness = b < 0.f ? 0.f : (b > 1.f ? 1.f : b); }
void Blink::update() {}
void Blink::setInterval(uint16_t on_ms, uint16_t off_ms) {
  blinkTimes[1] = on_ms;
  blinkTimes[0] = off_ms;
}

#else // Real ESP32

#include "driver/gpio.h"
#include "driver/ledc.h"

void Blink::init() {
  ledc_timer_config_t ledc_timer = {.speed_mode = LEDC_LOW_SPEED_MODE,
                                    .duty_resolution = LEDC_TIMER_13_BIT,
                                    .timer_num = LEDC_TIMER_0,
                                    .freq_hz = 5000,
                                    .clk_cfg = LEDC_AUTO_CLK};
  ledc_timer_config(&ledc_timer);

  ledc_channel_config_t ledc_channel = {.gpio_num = pin,
                                        .speed_mode = LEDC_LOW_SPEED_MODE,
                                        .channel = LEDC_CHANNEL_0,
                                        .intr_type = LEDC_INTR_DISABLE,
                                        .timer_sel = LEDC_TIMER_0,
                                        .duty = 0,
                                        .hpoint = 0};
  ledc_channel_config(&ledc_channel);
}

void Blink::updateTask(void *pvParameters) {
  Blink *blink = static_cast<Blink *>(pvParameters);
  while (1) {
    blink->update();
  }
}

void Blink::setBrightness(float b) {
  _brightness = b < 0.f ? 0.f : (b > 1.f ? 1.f : b);
}

void Blink::update() {
  uint32_t duty = bLedState ? (uint32_t)(_brightness * 8191) : 0;

  ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);

  vTaskDelay(blinkTimes[bLedState] / portTICK_PERIOD_MS);
  bLedState = !bLedState;
}

void Blink::setInterval(uint16_t on_ms, uint16_t off_ms) {
  blinkTimes[1] = on_ms;
  blinkTimes[0] = off_ms;
}

#endif // JFIX_EMULATION
