#include "JProtoExample.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include "simple.pb.h"
#include "esp_log.h"

static const char *TAG = "JProtoExample";

void JProtoExample::test() {
    uint8_t buffer[128];
    size_t message_length;
    bool status;

    /* Encoding */
    {
        Command cmd = Command_init_default;
        pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

        cmd.which_payload = Command_led_tag;
        cmd.payload.led.brightness = 0.5f;

        status = pb_encode(&stream, Command_fields, &cmd);
        message_length = stream.bytes_written;

        if (!status) {
            ESP_LOGE(TAG, "Encoding failed: %s", PB_GET_ERROR(&stream));
            return;
        }
        ESP_LOGI(TAG, "Encoded message length: %d", (int)message_length);
    }

    /* Decoding */
    {
        Command cmd = Command_init_default;
        pb_istream_t stream = pb_istream_from_buffer(buffer, message_length);

        status = pb_decode(&stream, Command_fields, &cmd);

        if (!status) {
            ESP_LOGE(TAG, "Decoding failed: %s", PB_GET_ERROR(&stream));
            return;
        }

        if (cmd.which_payload == Command_led_tag) {
            ESP_LOGI(TAG, "Decoded: LED Brightness=%.2f", cmd.payload.led.brightness);
        }
    }
}
