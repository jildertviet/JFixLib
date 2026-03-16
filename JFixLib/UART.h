#ifndef UART_H
#define UART_H

#ifndef JFIX_EMULATION

#include "driver/uart.h"
#include "esp_err.h"
#include <string>
#include <vector>
#include <unordered_map>

class UART {
public:
  UART();
  esp_err_t init();

private:
  static void uart_task(void *pvParameters);

  static const int BUF_SIZE = 1024;
  uart_port_t uart_num;
};

extern UART uartHandler;

#endif // !JFIX_EMULATION

#endif // UART_H
