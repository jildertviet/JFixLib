#include "jfixture.h"
#include "NVSStorage.h"
#include "OTAUpdater.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include <cstring>

static const char *TAG_JF = "jFixture";
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

jFixture::jFixture() {}

void jFixture::init() {
  nvs.init();
  connectWiFi();
  ota.checkForOTA();
}

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

  std::string ssid_str;
  std::string password_str;

  esp_err_t err_ssid = nvs.readString("ssid", ssid_str);
  esp_err_t err_pass = nvs.readString("password", password_str);

  if (err_ssid != ESP_OK || ssid_str.empty()) {
    ESP_LOGE(TAG_JF, "SSID not found in NVS. Cannot connect to WiFi.");
    return;
  }

  wifi_config_t wifi_config = {
      .sta = {.ssid = "",     // Will be filled below
              .password = "", // Will be filled below
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
    wifi_config.sta.threshold.authmode =
        WIFI_AUTH_WPA_WPA2_PSK; // Assuming WPA/WPA2 if password exists
  } else {
    wifi_config.sta.password[0] =
        '\0'; // Ensure password is null-terminated for open network
    wifi_config.sta.threshold.authmode = WIFI_AUTH_OPEN;
  }

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  ESP_LOGI(TAG_JF, "wifi_init_sta finished.");

  /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or
   * connection failed for the maximum number of re-tries (WIFI_FAIL_BIT). The
   * bits are set by event_handler() (located in this file) */
  EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                         WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                         pdFALSE, pdFALSE, portMAX_DELAY);

  /* xEventGroupWaitBits() returns the bits before the call returned, hence we
   * can use them to verify which event actually happened. */
  if (bits & WIFI_CONNECTED_BIT) {
    ESP_LOGI(TAG_JF, "connected to ap SSID:%s", ssid_str.c_str());
  } else if (bits & WIFI_FAIL_BIT) {
    ESP_LOGI(TAG_JF, "Failed to connect to SSID:%s", ssid_str.c_str());
  } else {
    ESP_LOGE(TAG_JF, "UNEXPECTED EVENT");
  }

  /* The event will not be processed after unregistering */
  ESP_ERROR_CHECK(esp_event_handler_instance_unregister(
      IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
  ESP_ERROR_CHECK(esp_event_handler_instance_unregister(
      WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
  vEventGroupDelete(s_wifi_event_group);
}
