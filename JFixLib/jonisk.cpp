#include "jonisk.h"
#include "BQ25792.h"
#include "I2CWrapper.h"

Jonisk::Jonisk() : charger(nullptr) {}

void Jonisk::init() {
  jFixture::init();
  blink.init();

  charger = new BQ25792(BQ_CE_PIN, BQ_INT_PIN);

  esp_err_t err = charger->begin();
  if (err == ESP_OK) {
    charger->enableCharging();
  }

  xTaskCreate(blink.updateTask, "blink", 2048, &blink, 0, NULL);
}
