#ifndef JFIXTURE_GRAPHICS_H
#define JFIXTURE_GRAPHICS_H

#include "jfixture.h"
#include "Event.h"
#include "JWavetable.h"
#include <vector>

#define MAX_EVENTS 128

class jFixtureGraphics : public jFixture {
public:
    jFixtureGraphics();
    virtual ~jFixtureGraphics();

    virtual void init() override;
    virtual void update() override;

    void addEvent(Event *e);
    Event *getEventByID(int id);
    void deleteEvents();
    void sync(int eventID);

    // Legacy binary data handlers (to be refactored to ProtoBuf later)
    void addEvent(const uint8_t *data, int data_len);
    void addEnv(const uint8_t *data, int data_len);
    void setVal(const uint8_t *data, int data_len);
    void setValN(const uint8_t *data, int data_len);
    void linkBus(const uint8_t *data, int data_len, float *b);
    void setCustomArg(const uint8_t *data, int data_len);

protected:
    Event *events[MAX_EVENTS];
    JWavetable *w;
    int lastIDAdded = -1;

    // Pointer to function for writing to LEDs, usually implemented in jFixtureAddr
    void (*writeRGBPtr)(int, float, float, float, uint8_t, floatColor **) = nullptr;
};

#endif // JFIXTURE_GRAPHICS_H
