#ifndef JEnv_h
#define JEnv_h

#include "Arduino.h"

enum JEnvState{
    IDLE,
    DONE,
    ACTIVE
};

class JEnv{
public:
    JEnv();
    ~JEnv();
    float update();
    // bool bActive = false;
    unsigned long stopTime = 0;
    unsigned long startTime = 0;
    unsigned short duration = 0;
    unsigned short a, s, r;
    float brightness = 0;
    void trigger(unsigned short a, unsigned short s, unsigned short r, float b, float bias=0);
    void trigger(float* ptr, unsigned short a, unsigned short s, unsigned short r, float b, float bias=0);
    float value = 0;
    float bias = 0; // To do
    float* writePtr = nullptr;
    char varName = '0';
    bool stateRandom = false;
    JEnvState state = IDLE;
    bool bKill = false;
};

#endif  // JEnv_h
