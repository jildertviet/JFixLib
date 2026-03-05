#include "espnow_handler.h"
#include "defines.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "parser.h"
#include "pb_encode.h"
#include <string.h>
#include <stdlib.h>

static const char *TAG = "EspnowHandler";

// Item posted to the receive queue from the WiFi-task callback.
// The callback mallocs the data buffer; the task frees it after processing.
struct espnow_recv_event_t {
    uint8_t *data;
    int len;
};

const uint8_t EspnowHandler::broadcast_mac[6] = {0xFF, 0xFF, 0xFF,
                                                 0xFF, 0xFF, 0xFF};

EspnowHandler::EspnowHandler() : recv_queue(nullptr) {}

EspnowHandler &EspnowHandler::getInstance() {
  static EspnowHandler instance;
  return instance;
}

esp_err_t EspnowHandler::init() {
  recv_queue = xQueueCreate(ESPNOW_QUEUE_SIZE, sizeof(espnow_recv_event_t));
  if (recv_queue == nullptr) {
    ESP_LOGE(TAG, "Failed to create receive queue");
    return ESP_ERR_NO_MEM;
  }

  // Set fixed channel for ESP-NOW. Requires STA to be disconnected from any AP first.
  esp_err_t ch_err = esp_wifi_set_channel(WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE);
  if (ch_err != ESP_OK) {
    ESP_LOGW(TAG, "Failed to set channel %d: %s", WIFI_CHANNEL, esp_err_to_name(ch_err));
  }

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

  // Spawn a task to process received packets outside the WiFi task context.
  // Stack 4096: enough for ProtoBuf decode + handler dispatch.
  xTaskCreate(receive_task, "espnow_recv", 4096,
              static_cast<void *>(&getInstance()), 5, nullptr);

  ESP_LOGI(TAG, "ESP-NOW initialized successfully");
  return ESP_OK;
}

// Called from the WiFi task — must not block or do heavy work.
// Copies the incoming bytes onto the heap and posts a pointer to the queue.
void EspnowHandler::receive_cb(const esp_now_recv_info_t *recv_info,
                               const uint8_t *data, int len) {
  if (data == nullptr || len <= 0) {
    return;
  }

  espnow_recv_event_t evt;
  evt.data = static_cast<uint8_t *>(malloc(len));
  if (evt.data == nullptr) {
    ESP_LOGE(TAG, "malloc failed for incoming packet (len=%d)", len);
    return;
  }
  memcpy(evt.data, data, len);
  evt.len = len;

  EspnowHandler &self = getInstance();
  if (xQueueSend(self.recv_queue, &evt, 0) != pdTRUE) {
    ESP_LOGW(TAG, "Receive queue full — dropping packet");
    free(evt.data);
  }
}

// Runs as a FreeRTOS task: drains the queue and dispatches to the Parser.
void EspnowHandler::receive_task(void *pvParameters) {
  EspnowHandler *self = static_cast<EspnowHandler *>(pvParameters);
  espnow_recv_event_t evt;

  while (xQueueReceive(self->recv_queue, &evt, portMAX_DELAY) == pdTRUE) {
    Parser::getInstance().processIncomingBuffer(evt.data, (size_t)evt.len);
    free(evt.data);
  }
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
