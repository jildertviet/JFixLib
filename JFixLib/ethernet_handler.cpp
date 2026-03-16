#ifndef JFIX_EMULATION
#include "ethernet_handler.h"
#include "parser.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_eth_driver.h"
#include "esp_eth_mac_spi.h"
#include "driver/spi_master.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "esp_netif_ip_addr.h"
#include <string.h>

static const char *TAG = "EthernetHandler";

EthernetHandler::EthernetHandler() {}

EthernetHandler &EthernetHandler::getInstance() {
    static EthernetHandler instance;
    return instance;
}

void EthernetHandler::eth_event_handler(void *arg, esp_event_base_t event_base,
                                        int32_t event_id, void *event_data) {
    EthernetHandler *self = static_cast<EthernetHandler *>(arg);

    if (event_base == ETH_EVENT && event_id == ETHERNET_EVENT_CONNECTED) {
        ESP_LOGI(TAG, "Ethernet link up");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_ETH_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));

        // Create UDP socket
        self->_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (self->_socket < 0) {
            ESP_LOGE(TAG, "Failed to create socket");
            return;
        }

        struct sockaddr_in bind_addr = {};
        bind_addr.sin_family = AF_INET;
        bind_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        bind_addr.sin_port = htons(ETH_UDP_PORT);

        if (bind(self->_socket, (struct sockaddr *)&bind_addr, sizeof(bind_addr)) < 0) {
            ESP_LOGE(TAG, "Failed to bind socket");
            close(self->_socket);
            self->_socket = -1;
            return;
        }

        self->_connected = true;
        ESP_LOGI(TAG, "UDP socket bound on port %d", ETH_UDP_PORT);

        // Spawn receive task
        xTaskCreate(udp_receive_task, "udp_recv", 4096, self, 5, nullptr);
    } else if (event_base == ETH_EVENT && event_id == ETHERNET_EVENT_DISCONNECTED) {
        ESP_LOGI(TAG, "Ethernet link down");
        self->_connected = false;
    }
}

void EthernetHandler::udp_receive_task(void *pvParameters) {
    EthernetHandler *self = static_cast<EthernetHandler *>(pvParameters);
    uint8_t buf[512];

    while (true) {
        struct sockaddr_in src_addr;
        socklen_t src_len = sizeof(src_addr);
        int len = recvfrom(self->_socket, buf, sizeof(buf), 0,
                           (struct sockaddr *)&src_addr, &src_len);
        if (len > 0) {
            Parser::getInstance().processIncomingBuffer(buf, (size_t)len);
        }
    }
}

esp_err_t EthernetHandler::init(uint8_t device_id) {
    _device_id = device_id;

    // Initialize SPI bus
    spi_bus_config_t buscfg = {};
    buscfg.miso_io_num = ETH_MISO_PIN;
    buscfg.mosi_io_num = ETH_MOSI_PIN;
    buscfg.sclk_io_num = ETH_SCLK_PIN;
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;

    esp_err_t err = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "SPI bus init failed: %s", esp_err_to_name(err));
        return err;
    }

    // SPI device config for W5500
    spi_device_interface_config_t spi_devcfg = {};
    spi_devcfg.mode = 0;
    spi_devcfg.clock_speed_hz = 33 * 1000 * 1000;
    spi_devcfg.spics_io_num = ETH_CS_PIN;
    spi_devcfg.queue_size = 20;

    // W5500 MAC config (polling mode, no INT pin)
    eth_w5500_config_t w5500_config = ETH_W5500_DEFAULT_CONFIG(SPI2_HOST, &spi_devcfg);
    w5500_config.int_gpio_num = -1;
    w5500_config.poll_period_ms = 10;

    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    phy_config.reset_gpio_num = ETH_RST_PIN;

    esp_eth_mac_t *mac = esp_eth_mac_new_w5500(&w5500_config, &mac_config);
    esp_eth_phy_t *phy = esp_eth_phy_new_w5500(&phy_config);

    esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_handle_t eth_handle = nullptr;

    err = esp_eth_driver_install(&eth_config, &eth_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "ETH driver install failed: %s", esp_err_to_name(err));
        return err;
    }

    // Create netif and attach
    esp_netif_config_t netif_cfg = ESP_NETIF_DEFAULT_ETH();
    _eth_netif = esp_netif_new(&netif_cfg);

    // Set static IP: 192.168.1.{device_id + 1} (offset by 1 since .0 is network addr)
    esp_netif_dhcpc_stop(_eth_netif);
    esp_netif_ip_info_t ip_info = {};
    uint8_t last_octet = device_id + 1;
    IP4_ADDR(&ip_info.ip, 192, 168, 1, last_octet);
    IP4_ADDR(&ip_info.gw, 192, 168, 1, 1);
    IP4_ADDR(&ip_info.netmask, 255, 255, 255, 0);
    esp_netif_set_ip_info(_eth_netif, &ip_info);
    ESP_LOGI(TAG, "Static IP: 192.168.1.%d", last_octet);

    void *glue = esp_eth_new_netif_glue(eth_handle);
    esp_netif_attach(_eth_netif, glue);

    // Register event handlers
    esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, this);
    esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &eth_event_handler, this);

    err = esp_eth_start(eth_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "ETH start failed: %s", esp_err_to_name(err));
        return err;
    }

    ESP_LOGI(TAG, "Ethernet init started");
    return ESP_OK;
}
#endif // !JFIX_EMULATION
