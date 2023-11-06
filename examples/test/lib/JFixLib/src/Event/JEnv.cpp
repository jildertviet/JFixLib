#include "JEnv.h"

JEnv::JEnv(){

}

float JEnv::update(){
    if(state == IDLE || state==DONE)
        return 0;
    float now = millis();

    if(millis() >= stopTime){
        Serial.println("Stop env");
        // bActive = false;
        state = DONE;
        value = bias;
        if(writePtr)
          *writePtr = value;
        return value;
    } else{
        // change val
        if(now < startTime + a){
            // Attack phase
            float ratio = (now - startTime) / ((float)a);
            value = ratio;
        } else if(now >= startTime + a && now < startTime + a + s){
            // Sustain phase
            if(!stateRandom){
                value = 1.;
            } else{
                int mod = 200;
                int s = millis() * millis();
                if(s % mod < mod * 0.3){
                    value = 0.11;
                } else if(s % mod > mod * 0.3 && s % mod < mod * 0.5){
                    value = 0.3;
                } else if(s % mod > mod * 0.5 && s % mod < mod * 0.65){
                    value = 1.0;
                } else{
                    value = 0;
                }
                delay(random(10));
            }
        } else if(now >= startTime + a + s && now < startTime + a + s + r){
//            Serial.println("rel");
            // Release phase
            float first = (now - (startTime + s + a));
            float ratio;

            if(first >= r){
                ratio = 1.0;
            } else{
                if((now - (startTime + s + a)) >= r){
                    ratio = 1.;
                } else{
                    float second = r * 1.05;
//                    Serial.println("first: " + String(first));
//                    Serial.println("second: " + String(second));
                    ratio = first / second;
                }
            }
            ratio = 1. - ratio;
            value = ratio;
            if(value < 0){
                value = 0;
            } else if(value > 1){
                value = 1;
            }
        }
    }
//    Serial.println("End of update()");
    if(writePtr)
        *writePtr = bias + (value * brightness);
    return bias + (value * brightness);
}

void JEnv::trigger(unsigned short a, unsigned short s, unsigned short r, float b, float bias){
    duration = a + s + r;
    this->a = a;
    this->s = s;
    this->r = r;
    // bActive = true;
    state = ACTIVE;
    startTime = millis();
    stopTime = startTime + duration;
    brightness = b;
    this->bias = bias;
}
void JEnv::trigger(float* ptr, unsigned short a, unsigned short s, unsigned short r, float b, float bias){
  writePtr = ptr;
  trigger(a, s, r, b, bias);
}
