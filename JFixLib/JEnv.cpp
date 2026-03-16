#include "JEnv.h"
#include "jfix_platform.h"

JEnv::JEnv() {}
JEnv::~JEnv() {}

uint32_t JEnv::getMillis() {
    return (uint32_t)(esp_timer_get_time() / 1000);
}

float JEnv::update() {
    if (state == IDLE || state == DONE)
        return 0;
    
    uint32_t now = getMillis();

    if (now >= stopTime) {
        state = DONE;
        value = bias;
        if (writePtr)
            *writePtr = value;
        return value;
    } else {
        if (now < startTime + a) {
            // Attack phase
            float ratio = (float)(now - startTime) / ((float)a);
            value = ratio;
        } else if (now >= startTime + a && now < startTime + a + s) {
            // Sustain phase
            value = 1.0f;
        } else if (now >= startTime + a + s && now < startTime + a + s + r) {
            // Release phase
            float first = (float)(now - (startTime + s + a));
            float ratio;

            if (first >= r) {
                ratio = 1.0f;
            } else {
                float second = (float)r * 1.05f;
                ratio = first / second;
            }
            ratio = 1.0f - ratio;
            value = ratio;
            if (value < 0) value = 0;
            else if (value > 1.0f) value = 1.0f;
        }
    }

    float finalVal = bias + (value * brightness);
    if (writePtr)
        *writePtr = finalVal;
    return finalVal;
}

void JEnv::trigger(uint16_t a, uint16_t s, uint16_t r, float b, float bias) {
    this->a = a;
    this->s = s;
    this->r = r;
    this->duration = a + s + r;
    this->state = ACTIVE;
    this->startTime = getMillis();
    this->stopTime = this->startTime + this->duration;
    this->brightness = b;
    this->bias = bias;
}

void JEnv::trigger(float *ptr, uint16_t a, uint16_t s, uint16_t r, float b, float bias) {
    writePtr = ptr;
    trigger(a, s, r, b, bias);
}
