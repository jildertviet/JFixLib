#include "Event.h"
#include "esp_timer.h"
#include <cstring>

ParameterConfig::ParameterConfig() {}

void ParameterConfig::init(float *b, float *ptr, char name) {
    this->busses = b;
    this->ptr = ptr;
    this->name = name;
}

void ParameterConfig::update() {
    if (bActive && busses && ptr) {
        *ptr = busses[id];
    }
}

void ParameterConfig::setFromBus() {
    if (busses && ptr) {
        *ptr = busses[id];
    }
}

Event::Event() {
    for (int i = 0; i < MAX_ENV; i++)
        envelopes[i] = nullptr;
    for (int i = 0; i < NUM_CUSTOM_ARGS; i++) {
        customBusses[i] = nullptr;
    }

    const char *names = "xywhrgBb";
    float *pointers[NUM_PARAMETER_CONFIGS];
    pointers[0] = &loc[0];
    pointers[1] = &loc[1];
    pointers[2] = &size[0];
    pointers[3] = &size[1];
    pointers[4] = &rgba[0];
    pointers[5] = &rgba[1];
    pointers[6] = &rgba[2];
    pointers[7] = &brightness;

    for (int i = 0; i < NUM_PARAMETER_CONFIGS; i++) {
        parameterConfigs[i].init(nullptr, pointers[i], names[i]);
    }
}

Event::~Event() {
    for (int i = 0; i < MAX_ENV; i++) {
        if (envelopes[i]) {
            delete envelopes[i];
            envelopes[i] = nullptr;
        }
    }
}

uint32_t Event::getMillis() {
    return (uint32_t)(esp_timer_get_time() / 1000);
}

void Event::linkBus(char name, char busIndex, float *b) {
    for (int i = 0; i < NUM_PARAMETER_CONFIGS; i++) {
        if (parameterConfigs[i].name == name) {
            parameterConfigs[i].busses = b;
            parameterConfigs[i].id = busIndex;
            parameterConfigs[i].bActive = true;
        }
    }
}

bool Event::checkLifeTime() {
    if ((endTime && getMillis() > endTime) || brightnessEnv.state == DONE) {
        bActive = false;
        return false;
    } else {
        return true;
    }
}

void Event::updateEnvelopes() {
    for (int i = 0; i < MAX_ENV; i++) {
        if (envelopes[i]) {
            envelopes[i]->update();
            if (envelopes[i]->state == DONE) {
                if (envelopes[i]->bKill)
                    bActive = false;
                delete envelopes[i];
                envelopes[i] = nullptr;
            }
        }
    }

    if (brightnessEnv.state != IDLE)
        brightness = brightnessEnv.update();
}

void Event::update() {
    for (int i = 0; i < NUM_PARAMETER_CONFIGS; i++) {
        parameterConfigs[i].update();
    }
    updateEnvelopes();
    checkLifeTime();
}

void Event::triggerBrightnessEnv(uint16_t a, uint16_t s, uint16_t r, float b) {
    brightnessEnv.trigger(a, s, r, b);
}

void Event::addEnv(char varName, float *dest, uint16_t a, uint16_t s,
                   uint16_t r, float b, float bias, bool bKill) {
    for (int i = 0; i < MAX_ENV; i++) {
        if (envelopes[i]) {
            if (envelopes[i]->varName == varName) {
                envelopes[i]->trigger(dest, a, s, r, b, bias);
                return;
            }
        }
    }
    for (int i = 0; i < MAX_ENV; i++) {
        if (envelopes[i] == nullptr) {
            envelopes[i] = new JEnv();
            envelopes[i]->trigger(dest, a, s, r, b, bias);
            envelopes[i]->bKill = bKill;
            envelopes[i]->varName = varName;
            return;
        }
    }
}

void Event::setVal(char type, float value) {
    switch (type) {
    case 'b': brightness = value; break;
    case 'x': loc[0] = value; break;
    case 'y': loc[1] = value; break;
    case 'w': size[0] = value; break;
    case 'h': size[1] = value; break;
    case 'r': rgba[0] = value; break;
    case 'g': rgba[1] = value; break;
    case 'B': rgba[2] = value; break;
    }
}

void Event::setCustomArg(uint8_t id, float val) {
    if (id < NUM_CUSTOM_ARGS && customBusses[id])
        *customBusses[id] = val;
}

void Event::setBusses(float *b, int num) {
    // Implementation can be added as needed
}
