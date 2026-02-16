#ifndef JENV_H
#define JENV_H

#include <stdint.h>

enum JEnvState {
    IDLE,
    DONE,
    ACTIVE
};

class JEnv {
public:
    JEnv();
    ~JEnv();
    float update();
    
    uint32_t stopTime = 0;
    uint32_t startTime = 0;
    uint16_t duration = 0;
    uint16_t a, s, r;
    float brightness = 0;
    
    void trigger(uint16_t a, uint16_t s, uint16_t r, float b, float bias = 0.0f);
    void trigger(float* ptr, uint16_t a, uint16_t s, uint16_t r, float b, float bias = 0.0f);
    
    float value = 0.0f;
    float bias = 0.0f;
    float* writePtr = nullptr;
    char varName = '0';
    bool stateRandom = false;
    JEnvState state = IDLE;
    bool bKill = false;

private:
    uint32_t getMillis();
};

#endif // JENV_H
