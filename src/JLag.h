#pragma once

class JLag{
  public:
    float lagTime = 1000; // ms
    unsigned long lagStartTime = 0;
    unsigned long lagEndTime = 0;
    float startVal = 0;
    float endVal = 0; // Perhaps call this 'targetVal'
    float* value;
    bool bLagDone = true;

    JLag(){};
    void update(){
      if(lagTime){
        if(millis() < lagEndTime){ // lagEndTime
          float ratio = (millis() - lagStartTime) / lagTime; // 0.0 - 1.0
          *value = (startVal * (1.0-ratio)) + (endVal * ratio);
        } else{
          if(!bLagDone){
            *value = endVal;
            bLagDone = true;
          }
        }
      } else{
        *value = endVal;
      }
    }
    void set(float v){
      lagStartTime = millis();
      lagEndTime = lagStartTime + lagTime;
      startVal = *value;
      endVal = v;
      bLagDone = false;
    }
    void link(float* v){
      value = v;
    }
};
