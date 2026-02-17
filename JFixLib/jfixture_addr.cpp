#include "jfixture_addr.h"
#include "esp_log.h"
#include "esp_timer.h"
#include <string.h>
#include <cmath>

static const char *TAG = "jFixtureAddr";

jFixtureAddr::jFixtureAddr() {
    this->writeRGBPtr = &jFixtureAddr::writeRGB;
}

jFixtureAddr::~jFixtureAddr() {
    if (ledBuffer) {
        for (int i = 0; i < numStrings; i++) {
            delete[] ledBuffer[i];
        }
        delete[] ledBuffer;
    }
    for (auto handle : ledStrips) {
        led_strip_del(handle);
    }
}

void jFixtureAddr::setup(uint8_t numColorChannels, const uint8_t *pins,
                         uint16_t numLedsPerString, JAddressableMode mode,
                         uint8_t numStrings) {
    this->numLedsPerString = numLedsPerString;
    this->numStrings = numStrings;
    this->ledMode = mode;
    this->viewport[1] = (float)numLedsPerString;

    ledBuffer = new floatColor *[numStrings];
    for (int i = 0; i < numStrings; i++) {
        ledBuffer[i] = new floatColor[numLedsPerString];
        memset(ledBuffer[i], 0, sizeof(floatColor) * numLedsPerString);

        if (pins) {
            led_strip_config_t strip_config = {
                .strip_gpio_num = pins[i],
                .max_leds = numLedsPerString,
                .led_model = LED_MODEL_WS2812,
                .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB,
                .flags = { .invert_out = false }
            };
            led_strip_rmt_config_t rmt_config = {
                .clk_src = RMT_CLK_SRC_DEFAULT,
                .resolution_hz = 10 * 1000 * 1000, // 10MHz
                .mem_block_symbols = 64,
                .flags = { .with_dma = false }
            };
            led_strip_handle_t strip;
            ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &strip));
            ledStrips.push_back(strip);
        }
    }

    initCurve();
    testLED();
    allBlack(true);
}

void jFixtureAddr::initCurve() {
    for (int i = 0; i < 256; i++) {
        brightnessCurve[i] = std::pow((i / 255.0f), 2.0f);
    }
}

void jFixtureAddr::writeRGB(int id, float r, float g, float b, uint8_t channel,
                            floatColor **leds) {
    if (!leds || !leds[channel]) return;
    
    if (r > leds[channel][id].r) leds[channel][id].r = r;
    if (g > leds[channel][id].g) leds[channel][id].g = g;
    if (b > leds[channel][id].b) leds[channel][id].b = b;
}

void jFixtureAddr::writeRGBHard(int id, float r, float g, float b, uint8_t channel,
                                floatColor **leds) {
    if (!leds || !leds[channel]) return;
    leds[channel][id].r = r;
    leds[channel][id].g = g;
    leds[channel][id].b = b;
}

void jFixtureAddr::testLED() {
    ESP_LOGI(TAG, "Starting LED test...");
    for (int h = 0; h < 3; h++) {
        for (int j = 0; j < numStrings; j++) {
            for (int i = 0; i < numLedsPerString; i++) {
                float c[3] = {0, 0, 0};
                c[h] = 1.0f;
                writeRGBHard(i, c[0], c[1], c[2], j, ledBuffer);
            }
        }
        writeLeds();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    ESP_LOGI(TAG, "LED test complete.");
}

void jFixtureAddr::allBlack(bool bWrite) {
    for (int j = 0; j < numStrings; j++) {
        for (int i = 0; i < numLedsPerString; i++) {
            writeRGBHard(i, 0, 0, 0, j, ledBuffer);
        }
    }
    if (bWrite) writeLeds();
}

void jFixtureAddr::writeLeds() {
    for (int j = 0; j < (int)ledStrips.size(); j++) {
        for (int i = 0; i < numLedsPerString; i++) {
            floatColor *c = &ledBuffer[j][i];
            uint32_t r = (uint32_t)(std::pow(c->r, 2.0f) * 255.0f * brightness);
            uint32_t g = (uint32_t)(std::pow(c->g, 2.0f) * 255.0f * brightness);
            uint32_t b = (uint32_t)(std::pow(c->b, 2.0f) * 255.0f * brightness);
            
            if (r > 255) r = 255;
            if (g > 255) g = 255;
            if (b > 255) b = 255;

            led_strip_set_pixel(ledStrips[j], i, r, g, b);
        }
        led_strip_refresh(ledStrips[j]);
    }
}

void jFixtureAddr::update() {
    jFixtureGraphics::update();
    
    if (bStatic) {
        for (int j = 0; j < numStrings; j++) {
            for (int i = 0; i < numLedsPerString; i++) {
                writeRGBHard(i, 1.0f, 1.0f, 1.0f, j, ledBuffer);
            }
        }
        writeLeds();
        return;
    }

    uint32_t now = (uint32_t)(esp_timer_get_time() / 1000);
    if (now - lastUpdatedLive < 20) return;
    lastUpdatedLive = now;

    allBlack();
    for (int i = 0; i < MAX_EVENTS; i++) {
        if (events[i]) {
            events[i]->update();
            if (events[i]->bActive) {
                events[i]->draw(ledBuffer, numLedsPerString, numStrings, horizontalPixelDistance);
            } else {
                delete events[i];
                events[i] = nullptr;
            }
        }
    }
    writeLeds();
}

void jFixtureAddr::blink(uint8_t num, uint16_t dur, uint16_t delayTime, uint8_t channel) {
    // Basic blink implementation
}
