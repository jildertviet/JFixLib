#ifndef ESPNOW_HANDLER_H
#define ESPNOW_HANDLER_H

#include <stdint.h>
#include <stddef.h>
#include "esp_err.h"
#include "esp_now.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "generated/simple.pb.h"

#define ESPNOW_QUEUE_SIZE 10

class EspnowHandler {
public:
    static EspnowHandler& getInstance();
    esp_err_t init();

    // Send a command to a specific MAC address (or broadcast if mac is all 0xFF)
    esp_err_t send(const Command& cmd, const uint8_t* target_mac = broadcast_mac);

private:
    EspnowHandler();
    static void receive_cb(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len);
    static void receive_task(void *pvParameters);

    static const uint8_t broadcast_mac[6];
    QueueHandle_t recv_queue;
};

#endif // ESPNOW_HANDLER_H
