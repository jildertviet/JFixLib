#include "jfixture_graphics.h"
#include "JEvent_Perlin.h"
#include "JOsc.h"
#include "JRect.h"
#include "jfix_platform.h"
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

// ---- Binary-format handlers (called from legacy path, kept until full ProtoBuf migration) ----

void jFixtureGraphics::addEvent(const uint8_t *data, int data_len) {
    if (data_len < 1) return;

    Event *e = nullptr;
    switch (data[0]) {
        case 0x01: { // JEvent_Perlin
            JEvent_Perlin *p = new JEvent_Perlin();
            if (data_len >= 1 + (int)sizeof(int))
                memcpy(&p->id, data + 1, sizeof(int));
            if (data_len >= 1 + (int)(sizeof(float) * 3))
                memcpy(&p->loc, data + 1 + sizeof(int), sizeof(float) * 2);
            if (data_len >= 1 + (int)(sizeof(float) * 5))
                memcpy(&p->size, data + 1 + sizeof(int) + sizeof(float) * 2, sizeof(float) * 2);
            if (data_len >= 1 + (int)(sizeof(float) * 9))
                memcpy(&p->rgba, data + 1 + sizeof(int) + sizeof(float) * 4, sizeof(float) * 4);
            if (data_len >= 1 + (int)(sizeof(float) * 9) + 1)
                memcpy(&p->bWaitForEnv, data + 1 + sizeof(int) + sizeof(float) * 9, 1);
            if (data_len >= 1 + 1 + (int)(sizeof(float) * 10))
                memcpy(&p->noiseScale, data + 1 + 1 + sizeof(int) + sizeof(float) * 9, sizeof(float));
            if (data_len >= 1 + 1 + (int)(sizeof(float) * 11))
                memcpy(&p->noiseTimeScale, data + 1 + 1 + sizeof(int) + sizeof(float) * 10, sizeof(float));
            if (data_len >= 1 + 1 + (int)(sizeof(float) * 12))
                memcpy(&p->horizontalPixelOffset, data + 1 + 1 + sizeof(int) + sizeof(float) * 11, sizeof(float));
            p->brightness = p->rgba[3];
            e = p;
            break;
        }
        case 0x02: { // JRect
            JRect *r = new JRect();
            if (data_len >= 1 + (int)sizeof(int))
                memcpy(&r->id, data + 1, sizeof(int));
            if (data_len >= 1 + (int)(sizeof(float) * 3))
                memcpy(&r->loc, data + 1 + sizeof(int), sizeof(float) * 2);
            if (data_len >= 1 + (int)(sizeof(float) * 5))
                memcpy(&r->size, data + 1 + sizeof(int) + sizeof(float) * 2, sizeof(float) * 2);
            if (data_len >= 1 + (int)(sizeof(float) * 9))
                memcpy(&r->rgba, data + 1 + sizeof(int) + sizeof(float) * 4, sizeof(float) * 4);
            if (data_len >= 1 + (int)(sizeof(float) * 9) + 1)
                memcpy(&r->bWaitForEnv, data + 1 + sizeof(int) + sizeof(float) * 9, 1);
            e = r;
            break;
        }
        case 0x03: { // JOsc
            JOsc *j = new JOsc(w);
            if (data_len >= 1 + (int)sizeof(int))
                memcpy(&j->id, data + 1, sizeof(int));
            if (data_len >= 1 + (int)(sizeof(float) * 3))
                memcpy(&j->loc, data + 1 + sizeof(int), sizeof(float) * 2);
            if (data_len >= 1 + (int)(sizeof(float) * 5))
                memcpy(&j->size, data + 1 + sizeof(int) + sizeof(float) * 2, sizeof(float) * 2);
            if (data_len >= 1 + (int)(sizeof(float) * 9))
                memcpy(&j->rgba, data + 1 + sizeof(int) + sizeof(float) * 4, sizeof(float) * 4);
            if (data_len >= 1 + (int)(sizeof(float) * 10))
                memcpy(&j->frequency, data + 1 + sizeof(int) + sizeof(float) * 9, sizeof(float));
            if (data_len >= 1 + (int)(sizeof(float) * 11))
                memcpy(&j->offset, data + 1 + sizeof(int) + sizeof(float) * 10, sizeof(float));
            if (data_len >= 1 + (int)(sizeof(float) * 12))
                memcpy(&j->range, data + 1 + sizeof(int) + sizeof(float) * 11, sizeof(float));
            if (data_len >= 1 + 1 + (int)(sizeof(float) * 12)) {
                float powVal;
                memcpy(&powVal, data + 1 + sizeof(int) + sizeof(float) * 12, sizeof(float));
                w->fillSineNorm(powVal);
            }
            if (data_len >= 1 + 1 + (int)(sizeof(float) * 13))
                memcpy(&j->bWaitForEnv, data + 1 + sizeof(int) + sizeof(float) * 13, 1);
            e = j;
            break;
        }
    }

    if (e) {
        addEvent(e);
    }
}

void jFixtureGraphics::addEnv(const uint8_t *data, int data_len) {
    // Binary layout: [float a, float s, float r, float b, char bKill, char varName, int eventID]
    if (data_len < (int)(sizeof(float) * 4 + 2 + sizeof(int))) return;

    float a, s, r, b;
    memcpy(&a, data,                    sizeof(float));
    memcpy(&s, data + sizeof(float),    sizeof(float));
    memcpy(&r, data + sizeof(float)*2,  sizeof(float));
    memcpy(&b, data + sizeof(float)*3,  sizeof(float));
    char bKill   = (char)data[sizeof(float) * 4];
    char varName = (char)data[sizeof(float) * 4 + 1];

    int eventID;
    memcpy(&eventID, data + sizeof(float) * 4 + 2, sizeof(int));

    Event *e = getEventByID(eventID);
    if (!e) {
        ESP_LOGW(TAG, "addEnv: no event with id %d", eventID);
        return;
    }

    switch (varName) {
        case 'b': e->addEnv('b', &e->brightness, (uint16_t)a, (uint16_t)s, (uint16_t)r, b, 0.0f, bKill); break;
        case 'x': e->addEnv('x', &e->loc[0],     (uint16_t)a, (uint16_t)s, (uint16_t)r, b, e->loc[0], bKill); break;
        case 'y': e->addEnv('y', &e->loc[1],     (uint16_t)a, (uint16_t)s, (uint16_t)r, b, e->loc[1], bKill); break;
        case 'w': e->addEnv('w', &e->size[0],    (uint16_t)a, (uint16_t)s, (uint16_t)r, b, e->size[0], bKill); break;
        case 'h': e->addEnv('h', &e->size[1],    (uint16_t)a, (uint16_t)s, (uint16_t)r, b, e->size[1], bKill); break;
    }
    e->updateEnvelopes();
    e->bWaitForEnv = false;
}

void jFixtureGraphics::setVal(const uint8_t *data, int data_len) {
    // Binary layout: [int eventID, char varName, float value]
    // For varName='c': [int eventID, char 'c', float r, float g, float b, float a]
    if (data_len < (int)(sizeof(int) + 1 + sizeof(float))) return;

    int eventID;
    char varName;
    float value;
    memcpy(&eventID, data,                      sizeof(int));
    memcpy(&varName, data + sizeof(int),        1);
    memcpy(&value,   data + sizeof(int) + 1,    sizeof(float));

    Event *e = getEventByID(eventID);
    if (!e) return;

    if (varName == 'c' && data_len >= (int)(sizeof(int) + 1 + sizeof(float) * 4)) {
        memcpy(&e->rgba, data + sizeof(int) + 1, sizeof(float) * 4);
    } else {
        e->setVal(varName, value);
    }
}

void jFixtureGraphics::setValN(const uint8_t *data, int data_len) {
    // Binary layout: [int eventID, char varName, float val0, float val1, ...]
    // This device picks value at index this->id
    if (data_len < (int)(sizeof(int) + 1 + sizeof(float))) return;

    int eventID;
    char varName;
    memcpy(&eventID, data,               sizeof(int));
    memcpy(&varName, data + sizeof(int), 1);

    int numValues = (data_len - sizeof(int) - 1) / sizeof(float);
    if (id < 0 || id >= numValues) return;

    float value;
    memcpy(&value, data + sizeof(int) + 1 + sizeof(float) * id, sizeof(float));

    Event *e = getEventByID(eventID);
    if (e) {
        e->setVal(varName, value);
    }
}

void jFixtureGraphics::setCustomArg(const uint8_t *data, int data_len) {
    // Binary layout: [int eventID, float argID, float value]
    if (data_len < (int)(sizeof(int) + sizeof(float) * 2)) return;

    int eventID;
    float argID, value;
    memcpy(&eventID, data,                          sizeof(int));
    memcpy(&argID,   data + sizeof(int),            sizeof(float));
    memcpy(&value,   data + sizeof(int) + sizeof(float), sizeof(float));

    Event *e = getEventByID(eventID);
    if (e) {
        e->setCustomArg((uint8_t)argID, value);
    }
}

void jFixtureGraphics::linkBus(const uint8_t *data, int data_len, float *b) {
    // Binary layout: [int eventID, char varName, char busIndex]
    if (data_len < (int)(sizeof(int) + 2)) return;

    int eventID;
    char varName, busIndex;
    memcpy(&eventID, data,               sizeof(int));
    memcpy(&varName, data + sizeof(int), 1);
    memcpy(&busIndex, data + sizeof(int) + 1, 1);

    Event *e = getEventByID(eventID);
    if (e) {
        e->linkBus(varName, busIndex, b);
    }
}
