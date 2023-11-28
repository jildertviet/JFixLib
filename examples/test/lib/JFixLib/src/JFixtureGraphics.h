#pragma once
#include "Event/Event.h"
#include "Event/JEvent_Perlin.h"
#include "Event/JOsc.h"
#include "Event/JRect.h"
#include "JEspnowDevice.h"

#define MAX_EVENTS 128

class JFixtureGraphics : public JEspnowDevice {
public:
  JFixtureGraphics() {
    memset(events, 0x00, sizeof(events));
    w = new JWavetable(1024);
    w->fillSineNorm(10);
  }
  Event *events[MAX_EVENTS];
  JWavetable *w;
  int lastIDAdded = -1;

  void (*writeRGBPtr)(int, float, float, float, char, floatColor **) =
      nullptr; // Will point to function of inherited class (JFixtureAddr)

  // void update() override{
  //   for(char i=0; i<MAX_EVENTS; i++){
  //     if(events[i]){
  //       events[i]->update();
  //       if(events[i]->bActive){
  //         // Serial.print("Active event: "); Serial.println((int)i);
  //         events[i]->draw(leds, numLedsPerString, numStrings);
  //         ...
  //       } else{
  //         Serial.println("delete events[i];");
  //         delete events[i];
  //         Serial.println("events[i] = nullptr;");
  //         events[i] = nullptr;
  //         Serial.println("done");
  //       }
  //     }
  //   }

  void addEvent(const uint8_t *data, int data_len) override {
    Event *e = nullptr;
    switch (data[0]) {
    case 0x01: { // Perlin
      e = (Event *)new JEvent_Perlin();
      // [noiseScale, noiseTimeScale, horizontalPixelDistance,
      // horizontalPixelOffset].asBytes32F ++ pos.asBytes32F ++ size.asBytes32F
      // ++ rgb ++ "end");
    } break;
    case 0x02: { // JRect
      Serial.println("R");
      JRect *r = new JRect(); // lifeTime
      memcpy(&(r->id), data + 1, sizeof(int) * 1);
      memcpy(&(r->loc), data + 1 + (sizeof(float) * 1), sizeof(float) * 2);
      memcpy(&(r->size), data + 1 + (sizeof(float) * 3), sizeof(float) * 2);
      memcpy(&(r->rgba), data + 1 + (sizeof(float) * 5), sizeof(float) * 4);
      e = (Event *)r;
      memcpy(&e->bWaitForEnv, data + 1 + (sizeof(float) * 9), 1);
      // e->canvas = &canvas; // Only link this in Events that actually use the
      // canvas
    } break;
    case 0x03: { // JOsc
      JOsc *j = new JOsc(w);
      memcpy(&(j->id), data + 1, sizeof(int) * 1);
      memcpy(&(j->loc), data + 1 + sizeof(float) * 1, sizeof(int) * 2);
      memcpy(&(j->size), data + 1 + sizeof(float) * 3, sizeof(int) * 2);
      memcpy(&(j->rgba), data + 1 + sizeof(float) * 5, sizeof(int) * 4);
      memcpy(&(j->frequency), data + 1 + sizeof(float) * 9, sizeof(int) * 1);
      memcpy(&(j->offset), data + 1 + sizeof(float) * 10, sizeof(int) * 1);
      memcpy(&(j->range), data + 1 + sizeof(float) * 11, sizeof(int) * 1);
      float powVal;
      memcpy(&powVal, data + 1 + sizeof(float) * 12, sizeof(int) * 1);
      memcpy(&j->bWaitForEnv, data + 1 + (sizeof(float) * 13), 1);
      w->fillSineNorm(powVal);
      e = (Event *)j;
    } break;
    }
    if (e) {
      e->viewport = viewport;
      e->viewportOffset = viewportOffset;
      e->writeRGB = this->writeRGBPtr; // Events use the static function of
                                       // their parent to write to the LEDs
      addEvent(e);
    }
  }

  void addEvent(Event *e) {
    if (checkDuplicateID(e))
      return;
    for (char i = 0; i < MAX_EVENTS; i++) {
      if (events[i] == nullptr) {
        e->bActive = true;
        lastIDAdded = e->id;
        events[i] = e;
        Serial.print("New event @: ");
        Serial.println((int)i);
        break;
      }
    }
  }

  bool checkDuplicateID(Event *e) {
    if (e->id == lastIDAdded)
      return true;
    for (int i = 0; i < MAX_EVENTS; i++) {
      if (events[i]) {
        if (e->id == events[i]->id)
          return true;
      }
    }
    return false;
  }

  void addEnv(const uint8_t *data, int data_len) override {
    float a, s, r, b;
    int id;
    memcpy(&a, data, sizeof(float));
    memcpy(&s, data + (sizeof(float) * 1), sizeof(float));
    memcpy(&r, data + (sizeof(float) * 2), sizeof(float));
    memcpy(&b, data + (sizeof(float) * 3), sizeof(float));

    char bKill =
        data[(sizeof(float) * 4)]; // Currently always deletes when done
    char bType = data[(sizeof(float) * 4) + 1];

    memcpy(&id, data + (sizeof(float) * 4) + 2, sizeof(float));
    Event *e = getEventByID(id);
    if (!e) {
      Serial.print("addEnv: Can't find Event w/ id: ");
      Serial.println(id);
      return;
    }
    switch (bType) {
    // case 'b': e->triggerBrightnessEnv(a, s, r, b, bKill); break;
    case 'b':
      e->addEnv('b', &e->brightness, a, s, r, b, 0.0, bKill);
      break;
    case 'x':
      e->addEnv('x', &e->loc[0], a, s, r, b, e->loc[0], bKill);
      break;
    case 'y':
      e->addEnv('y', &e->loc[1], a, s, r, b, e->loc[1], bKill);
      break;
    case 'w':
      e->addEnv('w', &e->size[0], a, s, r, b, e->size[0], bKill);
      break;
    case 'h':
      e->addEnv('h', &e->size[1], a, s, r, b, e->size[1], bKill);
      break;
    }
    e->updateEnvelopes();
    e->bWaitForEnv = false;
  }

  void setVal(const uint8_t *data, int data_len) override {
    // [id, type, value] : [423, 'b', 0.5] // Set brightness to 0.5 of Event w/
    // id 423
    int id;
    char type;
    float value;
    memcpy(&id, data, sizeof(int));
    memcpy(&type, data + sizeof(int), 1);
    memcpy(&value, data + sizeof(int) + 1, sizeof(float));
    // Serial.print("id, type, value: "); Serial.print(id); Serial.print(", ");
    // Serial.print(type); Serial.print(", "); Serial.println(value);
    Event *e = getEventByID(id);
    if (e) {
      switch (type) {
      case 'c': { // color
        if (data_len >= (sizeof(int) + 1 + (sizeof(float) * 4))) {
          memcpy(&(e->rgba), data + sizeof(int) + 1, sizeof(float) * 4);
        }
      } break;
      default:
        e->setVal(type, value);
        break;
      }
    }
  }

  void setCustomArg(const uint8_t *data, int data_len) override {
    int eventID;
    int argID;
    float val;
    memcpy(&id, data, sizeof(int));
    memcpy(&argID, data + (sizeof(int) * 1), sizeof(int));
    memcpy(&val, data + (sizeof(int) * 2), sizeof(int));
    Event *e = getEventByID(eventID);
    if (e) {
      e->setCustomArg(argID, val);
    }
  };

  void setValN(const uint8_t *data, int data_len) override {
    // [id, type, value, value, value] : [423, 'b', 0.5, 0.1, 0.4] // Set
    // brightness to 0.5 of Event w/ id 423 if this tlFix-id is 0
    int eventID;
    char type;
    char numValues = (data_len - sizeof(int) - sizeof(char)) / sizeof(float);
    float value;
    memcpy(&eventID, data, sizeof(int));
    memcpy(&type, data + sizeof(int), 1);
    memcpy(&value, data + sizeof(int) + 1 + (sizeof(float) * this->id),
           sizeof(float));
    // Serial.print("id, type, value: "); Serial.print(id); Serial.print(", ");
    // Serial.print(type); Serial.print(", "); Serial.println(value);
    Event *e = getEventByID(eventID);
    if (e) {
      if (this->id >= 0 && this->id < numValues) {
        e->setVal(type, value);
      }
    }
  }

  Event *getEventByID(int id) {
    for (int i = MAX_EVENTS - 1; i >= 0; i--) { // Reverse lookup
      if (events[i]) {
        if (events[i]->id == id) {
          return events[i];
        }
      }
    }
    return nullptr;
  }

  void deleteEvents() {
    for (int i = 0; i < MAX_EVENTS; i++) {
      if (events[i]) {
        delete events[i];
        events[i] = nullptr;
      }
    }
  }
};
