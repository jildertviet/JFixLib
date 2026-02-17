#include "parser.h"
#include "esp_log.h"
#include "NVSStorage.h"
#include "dimmer.h"
#include "jfixture.h"
#include "motor_controller.h"
#include <pb_decode.h>

static const char *TAG = "Parser";

Parser::Parser() {
    dispatcher[Command_led_tag] = handleLed;
    dispatcher[Command_channel_tag] = handleChannel;
    dispatcher[Command_wifi_tag] = handleWifi;
    dispatcher[Command_set_id_tag] = handleId;
    dispatcher[Command_motor_tag] = handleMotor;
}

Parser& Parser::getInstance() {
    static Parser instance;
    return instance;
}

void Parser::processIncomingBuffer(uint8_t* buffer, size_t size) {
    Command cmd = Command_init_default;
    pb_istream_t stream = pb_istream_from_buffer(buffer, size);

    if (pb_decode(&stream, Command_fields, &cmd)) {
        // ID Check: 0 is broadcast, otherwise must match my ID
        int myId = (jFixture::instance) ? jFixture::instance->getId() : 0;
        
        if (cmd.id != 0 && cmd.id != myId) {
            // Not for me
            return;
        }

        if (dispatcher.count(cmd.which_payload)) {
            dispatcher[cmd.which_payload](cmd);
        } else {
            ESP_LOGW(TAG, "Unknown command tag: %d", (int)cmd.which_payload);
        }
    } else {
        ESP_LOGE(TAG, "ProtoBuf decoding failed: %s", PB_GET_ERROR(&stream));
    }
}

void Parser::handleLed(const Command& cmd) {
    float val = cmd.payload.led.brightness;
    if (jFixture::instance) {
        jFixture::instance->setBrightness(val);
        ESP_LOGI(TAG, "Setting global brightness (lagged) to %.2f", val);
    } else {
        dimmer.setBrightness(val);
        dimmer.show();
        ESP_LOGI(TAG, "Setting global brightness (immediate) to %.2f", val);
    }
}

void Parser::handleChannel(const Command& cmd) {
    int ch = cmd.payload.channel.channel;
    float val = cmd.payload.channel.value;
    dimmer.setChannel(ch, val);
    dimmer.show();
    ESP_LOGI(TAG, "Setting channel %d to %.2f", ch, val);
}

void Parser::handleWifi(const Command& cmd) {
    const char* ssid = cmd.payload.wifi.ssid;
    const char* pass = cmd.payload.wifi.password;
    
    nvs.writeString("ssid", ssid);
    nvs.writeString("password", pass);
    
    if (strlen(pass) > 0) {
        ESP_LOGI(TAG, "WiFi credentials updated. SSID: %s. Restart to apply.", ssid);
    } else {
        ESP_LOGI(TAG, "WiFi SSID updated (open network). SSID: %s. Restart to apply.", ssid);
    }
}

void Parser::handleId(const Command& cmd) {
    int newId = cmd.payload.set_id.id;
    if (jFixture::instance) {
        jFixture::instance->setId(newId);
    }
}

void Parser::handleMotor(const Command& cmd) {
    int32_t steps = cmd.payload.motor.steps;
    float speed = cmd.payload.motor.speed;
    bool relative = cmd.payload.motor.relative;

    if (relative) {
        motorController.move(steps, speed);
        ESP_LOGI(TAG, "Motor move relative: %d steps at %.2f speed", (int)steps, speed);
    } else {
        motorController.moveTo(steps, speed);
        ESP_LOGI(TAG, "Motor move absolute: to %d at %.2f speed", (int)steps, speed);
    }
}
