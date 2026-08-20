#ifndef JFIX_EMULATION
#include "jonisk.h"
#include "BQ25792.h"
#include "I2CWrapper.h"
#include "NVSStorage.h"
#include "dimmer.h"
#include <cmath>
#include <cstdio>

// Compiled fall-back boot state used when NVS has no "boot_state" key yet
// (white channel half on). Overridden by the setBootState command.
#define JONISK_DEFAULT_W 0.5f

Jonisk::Jonisk() : charger(nullptr) {}

void Jonisk::init() {
  jFixture::init();
  blink.init();

  dimmer.init({16, 17, 18, 8}, true);
  // Restore the power-on colour/brightness saved by the setBootState command.
  // Format: "r,g,b,w,brightness". Falls back to a compiled default (white) when
  // NVS has no key yet, so a fresh device still lights up.
  {
    std::string s;
    float c[4] = {0.f, 0.f, 0.f, JONISK_DEFAULT_W};
    float bri = 1.0f;
    if (nvs.readString("boot_state", s) == ESP_OK && !s.empty()) {
      float p[5];
      if (sscanf(s.c_str(), "%f,%f,%f,%f,%f", &p[0], &p[1], &p[2], &p[3],
                 &p[4]) == 5) {
        c[0] = p[0];
        c[1] = p[1];
        c[2] = p[2];
        c[3] = p[3];
        bri = p[4];
      }
      ESP_LOGI("Jonisk", "Boot state loaded: %s", s.c_str());
    } else {
      ESP_LOGI("Jonisk", "No boot state in NVS, using default white");
    }
    for (int i = 0; i < 4; i++)
      dimmer.setChannel(i, c[i]);
    setBrightness(bri);
  }

  charger = new BQ25792(BQ_CE_PIN, BQ_INT_PIN);

  esp_err_t err = charger->begin();
  if (err == ESP_OK) {
#ifndef JFIX_DISABLE_BATTERY
    charger->enableCharging();
#else
    // Battery disabled at build time: keep the charger alive for monitoring,
    // but do not charge. BQ25792::update() enters ship mode on VBUS loss so the
    // unit powers off instead of running from the battery.
    ESP_LOGI("Jonisk", "Battery support disabled (JFIX_DISABLE_BATTERY)");
#endif

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

  // Command-driven output: the dimmer channels (colour) and brightness are set
  // by the led/channel/setBootState commands and simply applied here every
  // frame. The boot defaults are loaded from NVS in init(). Tilt-to-brightness
  // was experimental and is intentionally not applied; the accelerometer stays
  // initialised for future use.
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
