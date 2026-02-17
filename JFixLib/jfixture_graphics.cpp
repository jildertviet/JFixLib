#include "jfixture_graphics.h"
#include "JEvent_Perlin.h"
#include "JRect.h"
#include "esp_log.h"
#include "esp_timer.h"
#include <string.h>

static const char *TAG = "jFixtureGraphics";

jFixtureGraphics::jFixtureGraphics() {
    memset(events, 0, sizeof(events));
    w = new JWavetable(1024);
    w->fillSineNorm(10.0f);
}

jFixtureGraphics::~jFixtureGraphics() {
    deleteEvents();
    delete w;
}

void jFixtureGraphics::init() {
    jFixture::init();
}

void jFixtureGraphics::update() {
    jFixture::update();
}

void jFixtureGraphics::addEvent(Event *e) {
    if (!e) return;
    
    // Check for duplicate ID
    if (e->id == lastIDAdded) {
        delete e;
        return;
    }
    for (int i = 0; i < MAX_EVENTS; i++) {
        if (events[i] && events[i]->id == e->id) {
            delete e;
            return;
        }
    }

    for (int i = 0; i < MAX_EVENTS; i++) {
        if (events[i] == nullptr) {
            e->bActive = true;
            lastIDAdded = e->id;
            events[i] = e;
            e->viewport[0] = viewport[0];
            e->viewport[1] = viewport[1];
            e->viewportOffset[0] = viewportOffset[0];
            e->viewportOffset[1] = viewportOffset[1];
            e->writeRGB = this->writeRGBPtr;
            ESP_LOGI(TAG, "New event added at index %d, ID %d", i, e->id);
            break;
        }
    }
}

Event *jFixtureGraphics::getEventByID(int id) {
    for (int i = MAX_EVENTS - 1; i >= 0; i--) {
        if (events[i] && events[i]->id == id) {
            return events[i];
        }
    }
    return nullptr;
}

void jFixtureGraphics::deleteEvents() {
    for (int i = 0; i < MAX_EVENTS; i++) {
        if (events[i]) {
            delete events[i];
            events[i] = nullptr;
        }
    }
}

void jFixtureGraphics::sync(int eventID) {
    Event *e = getEventByID(eventID);
    if (e) {
        e->syncTime = (uint32_t)(esp_timer_get_time() / 1000);
    }
}

// These legacy handlers will be ported as needed or replaced by ProtoBuf commands.
void jFixtureGraphics::addEvent(const uint8_t *data, int data_len) {
    if (data_len < 1) return;
    
    Event *e = nullptr;
    switch (data[0]) {
        case 0x01: { // Perlin
            JEvent_Perlin *p = new JEvent_Perlin();
            if (data_len >= 1 + sizeof(int)) memcpy(&(p->id), data + 1, sizeof(int));
            if (data_len >= 1 + sizeof(int) + sizeof(float)*2) memcpy(&(p->loc), data + 1 + sizeof(int), sizeof(float) * 2);
            if (data_len >= 1 + sizeof(int) + sizeof(float)*4) memcpy(&(p->size), data + 1 + sizeof(int) + sizeof(float)*2, sizeof(float) * 2);
            if (data_len >= 1 + sizeof(int) + sizeof(float)*8) memcpy(&(p->rgba), data + 1 + sizeof(int) + sizeof(float)*4, sizeof(float) * 4);
            // ... truncated additional fields from main branch ...
            e = p;
            break;
        }
        case 0x02: { // JRect
            JRect *r = new JRect();
            if (data_len >= 1 + sizeof(int)) memcpy(&(r->id), data + 1, sizeof(int));
            if (data_len >= 1 + sizeof(int) + sizeof(float)*2) memcpy(&(r->loc), data + 1 + sizeof(int), sizeof(float) * 2);
            if (data_len >= 1 + sizeof(int) + sizeof(float)*4) memcpy(&(r->size), data + 1 + sizeof(int) + sizeof(float)*2, sizeof(float) * 2);
            if (data_len >= 1 + sizeof(int) + sizeof(float)*8) memcpy(&(r->rgba), data + 1 + sizeof(int) + sizeof(float)*4, sizeof(float) * 4);
            e = r;
            break;
        }
    }
    
    if (e) {
        addEvent(e);
    }
}

// Implement other legacy handlers similarly if required...
void jFixtureGraphics::addEnv(const uint8_t *data, int data_len) {}
void jFixtureGraphics::setVal(const uint8_t *data, int data_len) {}
void jFixtureGraphics::setValN(const uint8_t *data, int data_len) {}
void jFixtureGraphics::linkBus(const uint8_t *data, int data_len, float *b) {}
void jFixtureGraphics::setCustomArg(const uint8_t *data, int data_len) {}
