#include "UART.h"
#include "NVSStorage.h"
#include "dimmer.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "jfixture.h"
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>

static const char *TAG = "UART";

const std::unordered_map<std::string, UART::Command> UART::commandMap = {
    {"test", UART::Command::TEST},
    {"setchannel", UART::Command::SET_CHANNEL},
    {"setwifi", UART::Command::SET_WIFI},
    {"setbrightness", UART::Command::SET_BRIGHTNESS},
};

UART uartHandler;

UART::UART() : uart_num(UART_NUM_0) {}

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
  std::string buffer;

  while (1) {
    int len = uart_read_bytes(self->uart_num, data, BUF_SIZE - 1,
                              20 / portTICK_PERIOD_MS);
    if (len > 0) {
      for (int i = 0; i < len; i++) {
        char c = (char)data[i];
        if (c == ';' || c == '\n' || c == '\r') {
          if (!buffer.empty()) {
            self->handleMessage(buffer);
            buffer.clear();
          }
        } else {
          buffer += c;
        }
      }
    }
  }
  free(data);
  vTaskDelete(NULL);
}

void UART::handleMessage(const std::string &message) {
  std::string trimmed = message;
  // Basic trimming of whitespace
  trimmed.erase(trimmed.begin(), std::find_if(trimmed.begin(), trimmed.end(),
                                              [](unsigned char ch) {
                                                return !std::isspace(ch);
                                              }));
  trimmed.erase(std::find_if(trimmed.rbegin(), trimmed.rend(),
                             [](unsigned char ch) { return !std::isspace(ch); })
                    .base(),
                trimmed.end());

  if (trimmed.empty())
    return;

  size_t colonPos = trimmed.find(':');
  std::string command;
  std::vector<std::string> args;

  if (colonPos != std::string::npos) {
    command = trimmed.substr(0, colonPos);
    std::string argsStr = trimmed.substr(colonPos + 1);

    size_t start = 0;
    size_t end = argsStr.find(',');
    while (end != std::string::npos) {
      args.push_back(argsStr.substr(start, end - start));
      start = end + 1;
      end = argsStr.find(',', start);
    }
    args.push_back(argsStr.substr(start));
  } else {
    command = trimmed;
  }

  processCommand(command, args);
}

void UART::processCommand(const std::string &command,
                          const std::vector<std::string> &args) {
  auto it = commandMap.find(command);
  Command cmd = (it != commandMap.end()) ? it->second : Command::UNKNOWN;
  switch (cmd) {
  case Command::TEST: {
    ESP_LOGI(TAG, "Test command received! Args count: %d", (int)args.size());
    for (size_t i = 0; i < args.size(); i++) {
      ESP_LOGI(TAG, "  Arg[%d]: %s", (int)i, args[i].c_str());
    }
    break;
  }

  case Command::SET_CHANNEL: {
    if (args.size() >= 2) {
      int ch = atoi(args[0].c_str());
      float val = atof(args[1].c_str());
      dimmer.setChannel(ch, val);
      dimmer.show();
      ESP_LOGI(TAG, "Setting channel %d to %.2f", ch, val);
    } else {
      ESP_LOGW(TAG, "setchannel requires 2 arguments: channel,value");
    }
    break;
  }

  case Command::SET_WIFI: {
    if (args.size() >= 2) {
      nvs.writeString("ssid", args[0]);
      nvs.writeString("password", args[1]);
      ESP_LOGI(TAG, "WiFi credentials updated. SSID: %s. Restart to apply.",
               args[0].c_str());
    } else if (args.size() == 1) {
      nvs.writeString("ssid", args[0]);
      nvs.writeString("password", "");
      ESP_LOGI(TAG,
               "WiFi SSID updated (open network). SSID: %s. Restart to apply.",
               args[0].c_str());
    } else {
      ESP_LOGW(TAG,
               "setwifi requires at least SSID. Format: setwifi:SSID,PASSWORD");
    }
    break;
  }
  case Command::SET_BRIGHTNESS: {
    if (args.size() >= 1) {
      float val = atof(args[0].c_str());
      if (jFixture::instance) {
        jFixture::instance->setBrightness(val);
        ESP_LOGI(TAG, "Setting global brightness (lagged) to %.2f", val);
      } else {
        dimmer.setBrightness(val);
        dimmer.show();
        ESP_LOGI(TAG, "Setting global brightness (immediate) to %.2f", val);
      }
    } else {
      ESP_LOGW(TAG, "setbrightness requires 1 argument: value");
    }
    break;
  }
  default:

    ESP_LOGW(TAG, "Unknown command: %s", command.c_str());
    break;
  }
}
