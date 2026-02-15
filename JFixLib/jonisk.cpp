#include "jonisk.h"
#include "BQ25792.h"
#include "I2CWrapper.h"
#include "dimmer.h"

Jonisk::Jonisk() : charger(nullptr) {}

void Jonisk::init() {
  jFixture::init();
  blink.init();

  dimmer.init({1, 2, 3, 4});
  dimmer.test();

  charger = new BQ25792(BQ_CE_PIN, BQ_INT_PIN);

  esp_err_t err = charger->begin();
  if (err == ESP_OK) {
    charger->enableCharging();

    uint8_t status = 0;
    err = charger->getChargerStatus0(&status);
    if (err == ESP_OK) {
      ESP_LOGI("TEST", "Charger status register: 0x%02X", status);
      ESP_LOGI("TEST", "IINDPM_STAT:       %s",
               (status & (1 << 7)) ? "In Regulation" : "Normal");
      ESP_LOGI("TEST", "VINDPM_STAT:       %s",
               (status & (1 << 6)) ? "In Regulation" : "Normal");
      ESP_LOGI("TEST", "WD_STAT:           %s",
               (status & (1 << 5)) ? "Expired" : "Normal");
      ESP_LOGI("TEST", "POORSRC_STAT:      %s",
               (status & (1 << 4)) ? "Weak Adaptor" : "Normal");
      ESP_LOGI("TEST", "PG_STAT:           %s",
               (status & (1 << 3)) ? "Power Good" : "Not Power Good");
      ESP_LOGI("TEST", "AC2_PRESENT_STAT:  %s",
               (status & (1 << 2)) ? "Present" : "Not Present");
      ESP_LOGI("TEST", "AC1_PRESENT_STAT:  %s",
               (status & (1 << 1)) ? "Present" : "Not Present");
      ESP_LOGI("TEST", "VBUS_PRESENT_STAT: %s",
               (status & (1 << 0)) ? "Present" : "Not Present");
    } else {
      ESP_LOGE("TEST", "Failed to read charger status register: %s",
               esp_err_to_name(err));
    }
  }

  xTaskCreate(blink.updateTask, "blink", 2048, &blink, 0, NULL);
  xTaskCreate(updateTask, "jonisk_update", 4096, this, 5, NULL);
}

void Jonisk::update() {
  jFixture::update();
  dimmer.setBrightness(getBrightness());
  dimmer.show();
}

void Jonisk::updateTask(void *pvParameters) {
  Jonisk *self = static_cast<Jonisk *>(pvParameters);
  while (1) {
    self->update();
    vTaskDelay(pdMS_TO_TICKS(20)); // 50Hz update rate
  }
}
