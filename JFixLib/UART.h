#ifndef UART_H
#define UART_H

#include "driver/uart.h"
#include "esp_err.h"
#include <string>
#include <vector>
#include <unordered_map>

class UART {
public:
  UART();
  esp_err_t init();

  enum class Command {
    UNKNOWN,
    TEST,
    SET_CHANNEL,
    SET_WIFI,
    SET_BRIGHTNESS
  };

private:
  static void uart_task(void *pvParameters);
  void handleMessage(const std::string &message);
  void processCommand(const std::string &command, const std::vector<std::string> &args);

  static const std::unordered_map<std::string, Command> commandMap;

  static const int BUF_SIZE = 1024;
  uart_port_t uart_num;
};

extern UART uartHandler;

#endif // UART_H
