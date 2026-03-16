#include "NVSStorage.h"

NVSStorage nvs;

NVSStorage::NVSStorage(const char *nspace) : nvsNamespace(nspace) {}

#ifdef JFIX_EMULATION

// Emulation: NVS is a no-op (no persistent storage needed for visualizer)
esp_err_t NVSStorage::init() {
  bInit = true;
  return ESP_OK;
}

esp_err_t NVSStorage::writeString(const char *key, const std::string &value) {
  return ESP_OK;
}

esp_err_t NVSStorage::readString(const char *key, std::string &outValue) {
  outValue = "";
  return ESP_FAIL; // Key not found
}

#else // Real ESP32

#include "nvs.h"
#include "nvs_flash.h"
#include <vector>

esp_err_t NVSStorage::init() {
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
      err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);
  bInit = (err == ESP_OK);
  return err;
}

esp_err_t NVSStorage::writeString(const char *key, const std::string &value) {
  nvs_handle_t my_handle;
  esp_err_t err = nvs_open(nvsNamespace, NVS_READWRITE, &my_handle);
  if (err != ESP_OK) {
    return err;
  }

  err = nvs_set_str(my_handle, key, value.c_str());
  if (err == ESP_OK) {
    err = nvs_commit(my_handle);
  }

  nvs_close(my_handle);
  return err;
}

esp_err_t NVSStorage::readString(const char *key, std::string &outValue) {
  nvs_handle_t my_handle;
  esp_err_t err = nvs_open(nvsNamespace, NVS_READONLY, &my_handle);
  if (err != ESP_OK) {
    return err;
  }

  size_t required_size = 0;
  err = nvs_get_str(my_handle, key, NULL, &required_size);
  if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
    nvs_close(my_handle);
    return err;
  }

  if (required_size > 0) {
    std::vector<char> buf(required_size);
    err = nvs_get_str(my_handle, key, buf.data(), &required_size);
    if (err == ESP_OK) {
      outValue = std::string(buf.data());
    }
  } else {
    outValue = "";
  }

  nvs_close(my_handle);
  if (err == ESP_ERR_NVS_NOT_FOUND) {
    return ESP_OK;
  }

  return err;
}

#endif // JFIX_EMULATION
