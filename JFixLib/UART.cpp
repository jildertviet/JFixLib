#ifndef JFIX_EMULATION
#include "UART.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "parser.h"
#include <cstdlib>
#include <cstring>

static const char *TAG = "UART";

// Frame constants
static const uint8_t FRAME_START = 0xAA;
static const uint8_t FRAME_END = 0xBB;

UART uartHandler;

UART::UART() : uart_num(UART_NUM_0) {}

// Simple CRC16-CCITT implementation
uint16_t crc16_ccitt(const uint8_t *data, size_t len) {
  uint16_t crc = 0xFFFF;
  for (size_t i = 0; i < len; i++) {
    crc ^= (uint16_t)data[i] << 8;
    for (int j = 0; j < 8; j++) {
      if (crc & 0x8000)
        crc = (crc << 1) ^ 0x1021;
      else
        crc <<= 1;
    }
  }
  return crc;
}

esp_err_t UART::init() {
  uart_config_t uart_config = {
      .baud_rate = 115200,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
      .rx_flow_ctrl_thresh = 122,
      .source_clk = UART_SCLK_DEFAULT,
  };

  esp_err_t err = uart_param_config(uart_num, &uart_config);
  if (err != ESP_OK)
    return err;

  err = uart_driver_install(uart_num, BUF_SIZE * 2, 0, 0, NULL, 0);
  if (err != ESP_OK)
    return err;

  xTaskCreate(uart_task, "uart_task", 4096, this, 1, NULL);
  return ESP_OK;
}

void UART::uart_task(void *pvParameters) {
  UART *self = static_cast<UART *>(pvParameters);
  uint8_t *data = (uint8_t *)malloc(BUF_SIZE);

  while (1) {
    uint8_t start_byte;
    // 1. Sync: Wait for FRAME_START
    if (uart_read_bytes(self->uart_num, &start_byte, 1, portMAX_DELAY) != 1)
      continue;
    if (start_byte != FRAME_START)
      continue;

    // 2. Read Length
    uint8_t msg_len;
    if (uart_read_bytes(self->uart_num, &msg_len, 1, pdMS_TO_TICKS(100)) != 1) {
      ESP_LOGW(TAG, "Timeout reading length");
      continue;
    }

    if (msg_len >= BUF_SIZE) {
      ESP_LOGE(TAG, "Message too long: %d", msg_len);
      continue;
    }

    // 3. Read Payload
    if (uart_read_bytes(self->uart_num, data, msg_len, pdMS_TO_TICKS(100)) !=
        msg_len) {
      ESP_LOGW(TAG, "Timeout reading payload");
      continue;
    }

    // 4. Read CRC (2 bytes)
    uint8_t crc_bytes[2];
    if (uart_read_bytes(self->uart_num, crc_bytes, 2, pdMS_TO_TICKS(100)) !=
        2) {
      ESP_LOGW(TAG, "Timeout reading CRC");
      continue;
    }
    uint16_t received_crc = (crc_bytes[0] << 8) | crc_bytes[1];

    // 5. Read End Byte
    uint8_t end_byte;
    if (uart_read_bytes(self->uart_num, &end_byte, 1, pdMS_TO_TICKS(100)) !=
        1) {
      ESP_LOGW(TAG, "Timeout reading end byte");
      continue;
    }

    if (end_byte != FRAME_END) {
      ESP_LOGW(TAG, "Invalid end byte: 0x%02X", end_byte);
      continue;
    }

    // 6. Verify CRC
    uint16_t computed_crc = crc16_ccitt(data, msg_len);
    if (computed_crc != received_crc) {
      ESP_LOGE(TAG, "CRC Mismatch! Computed: 0x%04X, Received: 0x%04X",
               computed_crc, received_crc);
      continue;
    }

    // 7. Process valid message
    Parser::getInstance().processIncomingBuffer(data, (size_t)msg_len);
  }
  free(data);
  vTaskDelete(NULL);
}
#endif // !JFIX_EMULATION
