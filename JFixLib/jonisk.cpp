#ifndef JFIX_EMULATION
#include "jonisk.h"
#include "BQ25792.h"
#include "I2CWrapper.h"
#include "dimmer.h"
#include <cmath>

Jonisk::Jonisk() : charger(nullptr) {}

void Jonisk::init() {
  jFixture::init();
  blink.init();

  dimmer.init({16, 17, 18, 8});
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

  esp_err_t accel_err = accel.begin(MMA8451::Range::G2);
  if (accel_err != ESP_OK) {
    ESP_LOGE("Jonisk", "accel.begin() failed: %s", esp_err_to_name(accel_err));
  } else {
    ESP_LOGI("Jonisk", "MMA8451 ready");
  }
  xTaskCreate(blink.updateTask, "blink", 2048, &blink, 0, NULL);
  xTaskCreate(updateTask, "jonisk_update", 4096, this, 5, NULL);
}

void Jonisk::update() {
  jFixture::update();

  // White output: only channel 3 (W) on, R/G/B off
  dimmer.setChannel(0, 0.f);
  dimmer.setChannel(1, 0.f);
  dimmer.setChannel(2, 0.f);
  dimmer.setChannel(3, 0.5f);

  // Brightness from tilt angle: angle of the fixture from horizontal
  //   Z = +1g pointing straight up  → brightness = 1
  //   Z =  0g horizontal            → brightness = 0.5
  //   Z = -1g pointing straight down → brightness = 0
  static int dbg_count = 0;
  AccelData d;
  esp_err_t read_err = accel.read(d);
  if (read_err == ESP_OK) {
    float angle = atan2f(d.z, sqrtf(d.x * d.x + d.y * d.y));
    // angle: +π/2 (up) → 0 (horizontal) → -π/2 (down)
    float brightness = (angle + M_PI_2) / M_PI; // maps to [0, 1]
    brightness = fmaxf(0.f, fminf(1.f, brightness));
    setBrightness(brightness);
    if (++dbg_count >= 50) { // log ~once per second
      ESP_LOGI("Jonisk", "accel x=%.2f y=%.2f z=%.2f  angle=%.2f  brightness=%.2f",
               d.x, d.y, d.z, angle, brightness);
      dbg_count = 0;
    }
  } else {
    if (++dbg_count >= 50) {
      ESP_LOGE("Jonisk", "accel.read() failed: %s", esp_err_to_name(read_err));
      dbg_count = 0;
    }
  }

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
#endif // !JFIX_EMULATION
