#include "dimmer.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <cmath>

static const char *TAG = "Dimmer";

Dimmer dimmer;

Dimmer::Dimmer() {}

esp_err_t Dimmer::init(const std::vector<int> &pins) {
  _pins = pins;
  _channelValues.assign(_pins.size(), 0.0f);

  ledc_timer_config_t ledc_timer = {.speed_mode = MODE,
                                    .duty_resolution = RESOLUTION,
                                    .timer_num = TIMER,
                                    .freq_hz = FREQUENCY,
                                    .clk_cfg = LEDC_AUTO_CLK};
  esp_err_t err = ledc_timer_config(&ledc_timer);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to configure LEDC timer: %s", esp_err_to_name(err));
    return err;
  }

  for (size_t i = 0; i < _pins.size(); i++) {
    ledc_channel_config_t ledc_channel = {
        .gpio_num = (int)_pins[i],
        .speed_mode = MODE,
        .channel = (ledc_channel_t)(1 + i), // 0 is for blink
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = TIMER,
        .duty = 0,
        .hpoint = 0,
        .flags = {.output_invert = 0}};
    err = ledc_channel_config(&ledc_channel);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "Failed to configure LEDC channel %d: %s", i,
               esp_err_to_name(err));
      return err;
    }
  }

  ESP_LOGI(TAG, "Initialized with %d channels", (int)_pins.size());
  return ESP_OK;
}

void Dimmer::setChannel(int channel, float value) {
  if (channel >= 0 && channel < (int)_channelValues.size()) {
    _channelValues[channel] = value;
  }
}

void Dimmer::setBrightness(float brightness) { _globalBrightness = brightness; }

void Dimmer::show() {
  uint32_t max_duty = (1 << RESOLUTION) - 1;
  for (size_t i = 0; i < _channelValues.size(); i++) {
    float final_val = _channelValues[i] * _globalBrightness;
    if (final_val > 1.0f)
      final_val = 1.0f;
    if (final_val < 0.0f)
      final_val = 0.0f;

    uint32_t duty = (uint32_t)(final_val * max_duty);
    ledc_set_duty(MODE, (ledc_channel_t)(i + 1), duty);
    ledc_update_duty(MODE, (ledc_channel_t)(i + 1));
  }
}

void Dimmer::test() {
  ESP_LOGI(TAG, "Starting LED test sequence...");
  for (size_t i = 0; i < _pins.size(); i++) {
    ESP_LOGI(TAG, "Testing channel %d (GPIO %d)", i, _pins[i]);
    setChannel(i, 0.4f);
    show();
    vTaskDelay(pdMS_TO_TICKS(700));
    setChannel(i, 0.0f);
    show();
    vTaskDelay(pdMS_TO_TICKS(200));
  }
  ESP_LOGI(TAG, "LED test sequence complete.");
}

void Dimmer::blink(uint8_t num, uint16_t dur, uint16_t delayTime,
                   uint8_t channel) {
  for (int i = 0; i < num; i++) {
    setChannel(channel, 0.8f);
    show();
    vTaskDelay(pdMS_TO_TICKS(dur));
    setChannel(channel, 0.0f);
    show();
    vTaskDelay(pdMS_TO_TICKS(delayTime));
  }
}
