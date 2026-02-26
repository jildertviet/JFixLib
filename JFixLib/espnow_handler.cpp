#include "espnow_handler.h"
#include "defines.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "parser.h"
#include "pb_encode.h"
#include <string.h>

static const char *TAG = "EspnowHandler";

const uint8_t EspnowHandler::broadcast_mac[6] = {0xFF, 0xFF, 0xFF,
                                                 0xFF, 0xFF, 0xFF};

EspnowHandler::EspnowHandler() {}

EspnowHandler &EspnowHandler::getInstance() {
  static EspnowHandler instance;
  return instance;
}

esp_err_t EspnowHandler::init() {
  // Ensure WiFi is initialized and channel is set
  ESP_ERROR_CHECK(esp_wifi_set_channel(WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE));

  esp_err_t err = esp_now_init();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error initializing ESP-NOW: %s", esp_err_to_name(err));
    return err;
  }

  esp_now_register_recv_cb(receive_cb);

  // Add broadcast peer
  esp_now_peer_info_t peer = {};
  memcpy(peer.peer_addr, broadcast_mac, 6);
  peer.channel = 0; // Use current channel
  peer.encrypt = false;

  err = esp_now_add_peer(&peer);
  if (err != ESP_OK && err != ESP_ERR_ESPNOW_EXIST) {
    ESP_LOGE(TAG, "Error adding broadcast peer: %s", esp_err_to_name(err));
    return err;
  }

  ESP_LOGI(TAG, "ESP-NOW initialized successfully");
  return ESP_OK;
}

void EspnowHandler::receive_cb(const esp_now_recv_info_t *recv_info,
                               const uint8_t *data, int len) {
  // Pass raw ProtoBuf data to the Parser
  Parser::getInstance().processIncomingBuffer((uint8_t *)data, (size_t)len);
}

esp_err_t EspnowHandler::send(const Command &cmd, const uint8_t *target_mac) {
  uint8_t buffer[Command_size];
  pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

  if (!pb_encode(&stream, Command_fields, &cmd)) {
    ESP_LOGE(TAG, "Encoding failed: %s", PB_GET_ERROR(&stream));
    return ESP_FAIL;
  }

  esp_err_t err = esp_now_send(target_mac, buffer, stream.bytes_written);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Send failed: %s", esp_err_to_name(err));
  }
  return err;
}
