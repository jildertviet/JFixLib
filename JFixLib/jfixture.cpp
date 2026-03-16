#include "jfixture.h"
#include "jfix_platform.h"
#include "NVSStorage.h"
#include <cstring>

#ifndef JFIX_EMULATION
#include "espnow_handler.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_mac.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#endif

#ifdef JFIX_ENABLE_UART
#include "UART.h"
#endif

static const char *TAG_JF = "jFixture";

#ifndef JFIX_EMULATION
static EventGroupHandle_t s_wifi_event_group;
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data) {
  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
    esp_wifi_connect();
  } else if (event_base == WIFI_EVENT &&
             event_id == WIFI_EVENT_STA_DISCONNECTED) {
    esp_wifi_connect();
    ESP_LOGI(TAG_JF, "retry to connect to the AP");
    xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);

  } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    ESP_LOGI(TAG_JF, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
    xEventGroupClearBits(s_wifi_event_group, WIFI_FAIL_BIT);
    xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
  }
}
#endif // !JFIX_EMULATION

jFixture *jFixture::instance = nullptr;

jFixture::jFixture() {
  instance = this;
  laggers[0].link(&brightness);
  brightnessLag = &laggers[0];
}

void jFixture::init() {
#ifdef JFIX_ENABLE_UART
  uartHandler.init();
#endif

#ifndef JFIX_EMULATION
  nvs.init();

  std::string id_str;
  if (nvs.readString("device_id", id_str) == ESP_OK) {
    id = atoi(id_str.c_str());
    ESP_LOGI(TAG_JF, "Device ID loaded from NVS: %d", id);
  } else {
    id = 0;
    ESP_LOGI(TAG_JF, "Device ID not found, defaulting to 0");
  }

  connectWiFi();

  esp_efuse_mac_get_default(baseMac);
  esp_wifi_get_mac(WIFI_IF_STA, staMac);
  ESP_LOGI(TAG_JF, "Base MAC: %02x:%02x:%02x:%02x:%02x:%02x  STA MAC: %02x:%02x:%02x:%02x:%02x:%02x",
           baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5],
           staMac[0], staMac[1], staMac[2], staMac[3], staMac[4], staMac[5]);

#ifdef JFIX_ENABLE_OTA
  ota.checkForOTA();
#endif
  esp_wifi_disconnect();
  EspnowHandler::getInstance().init();
#else
  // Emulation: just default the ID
  id = 0;
  ESP_LOGI(TAG_JF, "Emulation mode — device ID = %d", id);
#endif // !JFIX_EMULATION
}

void jFixture::update() {
  updateLaggers();
}

void jFixture::setBrightness(float b) {
  brightnessLag->set(b);
}

float jFixture::getBrightness() {
  return brightness;
}

bool jFixture::macMatches(const uint8_t* other, size_t len) const {
  if (len != 6) return false;
  return memcmp(other, baseMac, 6) == 0 || memcmp(other, staMac, 6) == 0;
}

void jFixture::setId(int newId) {
  id = newId;
#ifndef JFIX_EMULATION
  nvs.writeString("device_id", std::to_string(newId));
#endif
  ESP_LOGI(TAG_JF, "Device ID updated to %d", id);
}

void jFixture::setParameterBus(int index, float value) {
  if (index >= 0 && index < NUM_PARAMETER_BUSSES) {
    parameterBusses[index] = value;
  }
}

void jFixture::setParameterBusN(int startIndex, const float* values, int count) {
  for (int i = 0; i < count; i++) {
    int idx = startIndex + i;
    if (idx >= NUM_PARAMETER_BUSSES) break;
    parameterBusses[idx] = values[i];
  }
}

void jFixture::setBackground(float r, float g, float b, float a) {
    rgbaBackground[0] = r;
    rgbaBackground[1] = g;
    rgbaBackground[2] = b;
    rgbaBackground[3] = a;
}

void jFixture::setViewportOffset(float x, float y) {
    viewportOffset[0] = x;
    viewportOffset[1] = y;
}

void jFixture::setLagTime(int lagger_id, float lag_ms) {
  if (lagger_id >= 0 && lagger_id < NUM_LAGGERS) {
    laggers[lagger_id].lagTime = lag_ms;
  }
}

void jFixture::updateLaggers() {
  for (int i = 0; i < NUM_LAGGERS; i++) {
    laggers[i].update();
  }
}

#ifndef JFIX_EMULATION
void jFixture::connectWiFi() {
  s_wifi_event_group = xEventGroupCreate();

  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_sta();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  esp_event_handler_instance_t instance_any_id;
  esp_event_handler_instance_t instance_got_ip;
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_any_id));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, &instance_got_ip));

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_start());

  std::string ssid_str;
  std::string password_str;

  esp_err_t err_ssid = nvs.readString("ssid", ssid_str);
  esp_err_t err_pass = nvs.readString("password", password_str);

  if (err_ssid != ESP_OK || ssid_str.empty()) {
    ESP_LOGE(TAG_JF, "SSID not found in NVS. Station connection skipped (ESP-NOW will still work).");
    return;
  }

  wifi_config_t wifi_config = {
      .sta = {.ssid = "",
              .password = "",
              .scan_method = WIFI_ALL_CHANNEL_SCAN,
              .bssid_set = false,
              .bssid = {0},
              .channel = 0,
              .listen_interval = 3,
              .sort_method = WIFI_CONNECT_AP_BY_SIGNAL,
              .threshold =
                  {
                      .rssi = -127,
                      .authmode = WIFI_AUTH_OPEN,
                  },
              .pmf_cfg = {.capable = true, .required = false},
              .rm_enabled = true,
              .btm_enabled = true,
              .mbo_enabled = true,
              .transition_disable = false,
              .reserved = 0},
  };

  strncpy((char *)wifi_config.sta.ssid, ssid_str.c_str(),
          sizeof(wifi_config.sta.ssid) - 1);
  if (!password_str.empty()) {
    strncpy((char *)wifi_config.sta.password, password_str.c_str(),
            sizeof(wifi_config.sta.password) - 1);
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA_WPA2_PSK;
  } else {
    wifi_config.sta.password[0] = '\0';
    wifi_config.sta.threshold.authmode = WIFI_AUTH_OPEN;
  }

  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
  esp_wifi_connect();

  ESP_LOGI(TAG_JF, "wifi_init_sta finished.");

  EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                         WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                         pdFALSE, pdFALSE, portMAX_DELAY);

  if (bits & WIFI_CONNECTED_BIT) {
    ESP_LOGI(TAG_JF, "connected to ap SSID:%s", ssid_str.c_str());
  } else if (bits & WIFI_FAIL_BIT) {
    ESP_LOGI(TAG_JF, "Failed to connect to SSID:%s", ssid_str.c_str());
  } else {
    ESP_LOGE(TAG_JF, "UNEXPECTED EVENT");
  }

  ESP_ERROR_CHECK(esp_event_handler_instance_unregister(
      IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
  ESP_ERROR_CHECK(esp_event_handler_instance_unregister(
      WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
  vEventGroupDelete(s_wifi_event_group);
}
#else
void jFixture::connectWiFi() {
  // No WiFi in emulation mode
}
#endif // !JFIX_EMULATION
