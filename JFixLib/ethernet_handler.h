#ifndef ETHERNET_HANDLER_H
#define ETHERNET_HANDLER_H

#ifndef JFIX_EMULATION

#include "esp_err.h"
#include "esp_eth.h"
#include "esp_netif.h"
#include <stdint.h>

// Default W5500 SPI pins (ESP32 SPI2 defaults from original Arduino project)
#ifndef ETH_MISO_PIN
#define ETH_MISO_PIN  19
#endif
#ifndef ETH_MOSI_PIN
#define ETH_MOSI_PIN  23
#endif
#ifndef ETH_SCLK_PIN
#define ETH_SCLK_PIN  18
#endif
#ifndef ETH_CS_PIN
#define ETH_CS_PIN    21
#endif
#ifndef ETH_RST_PIN
#define ETH_RST_PIN   25
#endif
#ifndef ETH_UDP_PORT
#define ETH_UDP_PORT  1111
#endif

class EthernetHandler {
public:
    static EthernetHandler& getInstance();
    esp_err_t init(uint8_t device_id);
    bool isConnected() const { return _connected; }

private:
    EthernetHandler();

    static void eth_event_handler(void *arg, esp_event_base_t event_base,
                                  int32_t event_id, void *event_data);
    static void udp_receive_task(void *pvParameters);

    bool _connected = false;
    uint8_t _device_id = 0;
    int _socket = -1;
    esp_netif_t *_eth_netif = nullptr;
};

#endif // !JFIX_EMULATION

#endif // ETHERNET_HANDLER_H
