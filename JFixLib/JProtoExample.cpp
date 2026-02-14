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
        SimpleMessage message = SimpleMessage_init_default;
        pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

        message.id = 42;
        snprintf(message.text, sizeof(message.text), "Hello from Nanopb!");
        message.active = true;

        status = pb_encode(&stream, SimpleMessage_fields, &message);
        message_length = stream.bytes_written;

        if (!status) {
            ESP_LOGE(TAG, "Encoding failed: %s", PB_GET_ERROR(&stream));
            return;
        }
        ESP_LOGI(TAG, "Encoded message length: %d", (int)message_length);
    }

    /* Decoding */
    {
        SimpleMessage message = SimpleMessage_init_default;
        pb_istream_t stream = pb_istream_from_buffer(buffer, message_length);

        status = pb_decode(&stream, SimpleMessage_fields, &message);

        if (!status) {
            ESP_LOGE(TAG, "Decoding failed: %s", PB_GET_ERROR(&stream));
            return;
        }

        ESP_LOGI(TAG, "Decoded: ID=%d, Text=%s, Active=%s", 
                 (int)message.id, message.text, message.active ? "true" : "false");
    }
}
