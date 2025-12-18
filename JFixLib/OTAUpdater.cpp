#include "OTAUpdater.h"
#include "NVSStorage.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"

static const char *TAG = "OTAUpdater";

OTAUpdater::OTAUpdater() {}

void OTAUpdater::checkForOTA() {
  ESP_LOGI(TAG, "Checking for OTA update from NVS...");
  if (nvs.bInit == false) {
    esp_err_t err = nvs.init();
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "Failed to initialize NVS: %s", esp_err_to_name(err));
      return;
    }
  }

  std::string ota_url;
  esp_err_t err = nvs.readString("OTAurl", ota_url);

  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to read OTA URL from NVS: %s", esp_err_to_name(err));
    return;
  }

  if (ota_url.empty()) {
    ESP_LOGI(TAG, "OTA URL not found in NVS. Skipping OTA update.");
    return;
  }

  ESP_LOGI(TAG, "Found OTA URL: %s", ota_url.c_str());
  ESP_LOGI(TAG, "Starting OTA update...");

  esp_http_client_config_t config = {
      .url = ota_url.c_str(),
      .cert_pem = NULL,
      .timeout_ms = 5000,
      .keep_alive_enable = true,
  };

  esp_https_ota_config_t ota_config = {
      .http_config = &config,
  };

  err = esp_https_ota(&ota_config);

  if (err == ESP_OK) {
    ESP_LOGI(TAG, "OTA Update Successful, rebooting...");
    esp_restart();
  } else {
    ESP_LOGE(TAG, "OTA Update failed: %s", esp_err_to_name(err));
  }
}
