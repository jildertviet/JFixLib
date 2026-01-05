#include "blink.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

Blink blink;

Blink::Blink() {}

void Blink::init() {
  // Prepare and then apply the LEDC PWM timer configuration
  ledc_timer_config_t ledc_timer = {.speed_mode = LEDC_LOW_SPEED_MODE,
                                    .duty_resolution = LEDC_TIMER_13_BIT,
                                    .timer_num = LEDC_TIMER_0,
                                    .freq_hz =
                                        5000, // Set output frequency at 5 kHz
                                    .clk_cfg = LEDC_AUTO_CLK};
  ledc_timer_config(&ledc_timer);

  // Prepare and then apply the LEDC PWM channel configuration
  ledc_channel_config_t ledc_channel = {.gpio_num = pin,
                                        .speed_mode = LEDC_LOW_SPEED_MODE,
                                        .channel = LEDC_CHANNEL_0,
                                        .intr_type = LEDC_INTR_DISABLE,
                                        .timer_sel = LEDC_TIMER_0,
                                        .duty = 0, // Set duty to 0%
                                        .hpoint = 0};
  ledc_channel_config(&ledc_channel);
}

void Blink::updateTask(void *pvParameters) {
  Blink *blink = static_cast<Blink *>(pvParameters);
  while (1) {
    blink->update();
  }
}

void Blink::update() {
  // 100% of 2^13 (8192)
  uint32_t duty = bLedState ? 200 : 0;

  ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);

  vTaskDelay(blinkTimes[bLedState] / portTICK_PERIOD_MS);
  bLedState = !bLedState;
}
