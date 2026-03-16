#include "lagger.h"
#include "jfix_platform.h"

Lagger::Lagger() {}

uint32_t Lagger::getMillis() {
    return (uint32_t)(esp_timer_get_time() / 1000);
}

void Lagger::update() {
    if (!value) return;

    if (lagTime > 0) {
        uint32_t now = getMillis();
        if (now < lagEndTime) {
            float ratio = (float)(now - lagStartTime) / lagTime;
            if (ratio > 1.0f) ratio = 1.0f;
            *value = (startVal * (1.0f - ratio)) + (endVal * ratio);
        } else {
            if (!bLagDone) {
                *value = endVal;
                bLagDone = true;
            }
        }
    } else {
        *value = endVal;
        bLagDone = true;
    }
}

void Lagger::set(float v) {
    if (!value) return;
    lagStartTime = getMillis();
    lagEndTime = lagStartTime + (uint32_t)lagTime;
    startVal = *value;
    endVal = v;
    bLagDone = false;
}

void Lagger::link(float *v) {
    value = v;
}
