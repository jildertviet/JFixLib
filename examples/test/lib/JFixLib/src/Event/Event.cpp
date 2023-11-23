#include "Event.h"

Event::Event() {
  for (int i = 0; i < MAX_ENV; i++)
    envelopes[i] = nullptr;
}

void Event::testFunc(){

};

Event::~Event() {
  //  Serial.println("Begin ~Event");
  //  delete[] loc;
  //  delete[] size;
  //  delete[] color;
  //  delete[] speed;
  //  Serial.println("End ~Event");
}

bool Event::checkLifeTime() {
  if (endTime && millis() > endTime || brightnessEnv.state == DONE) {
    bActive = false;
    return false;
  } else {
    return true;
  }
}

void Event::updateEnvelopes() {
  for (int i = 0; i < MAX_ENV; i++) {
    if (envelopes[i]) {
      envelopes[i]->update(); // Writes to float ptr
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

void Event::triggerBrightnessEnv(unsigned short a, unsigned short s,
                                 unsigned short r, float b) {
  brightnessEnv.trigger(a, s, r, b);
}

void Event::addEnv(char varName, float *dest, unsigned short a,
                   unsigned short s, unsigned short r, float b, float bias,
                   bool bKill) {
  // Check if another env is writing to the same value, if so: simply retrigger
  // that one.
  for (int i = 0; i < MAX_ENV; i++) {
    if (envelopes[i]) {
      if (envelopes[i]->varName == varName) {
        envelopes[i]->trigger(dest, a, s, r, b,
                              envelopes[i]->bias); // Retrigger
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
  case 'b':
    brightness = value;
    break;
  case 'x':
    loc[0] = value;
    break;
  case 'y':
    loc[1] = value;
    break;
  case 'w':
    size[0] = value;
    break;
  case 'h':
    size[1] = value;
    break;
  }
}
