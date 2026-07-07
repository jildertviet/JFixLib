#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>
#include <stddef.h>
#include <map>
#include "generated/simple.pb.h"

#ifndef JFIX_EMULATION
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#endif

#define PARSER_CMD_QUEUE_SIZE 10

class Parser {
public:
    static Parser& getInstance();

    // Parse one or more length-prefixed ProtoBuf commands from a buffer.
    // Format: [2-byte big-endian len][Command bytes] repeated.
    void processIncomingBuffer(uint8_t* buffer, size_t size);

    // Drain queued graphics commands. Must be called from the render task
    // (e.g. jFixtureAddr::update) so that event state is only touched from
    // one task at a time.
    void drainCommandQueue();

private:
    Parser();
    void dispatchCommand(const Command& cmd);
    bool isGraphicsCommand(pb_size_t tag) const;

    typedef void (*HandlerFunc)(const Command& cmd);
    std::map<pb_size_t, HandlerFunc> dispatcher;

    // Simple control
    static void handleLed(const Command& cmd);
    static void handleChannel(const Command& cmd);
    static void handleWifi(const Command& cmd);
    static void handleId(const Command& cmd);
    static void handleBlink(const Command& cmd);
    static void handleSleep(const Command& cmd);
    static void handleLag(const Command& cmd);
    static void handleReboot(const Command& cmd);
    static void handleSetOtaUrl(const Command& cmd);
    static void handleSetBackground(const Command& cmd);
    static void handleSetViewportOffset(const Command& cmd);
    static void handleSetBootState(const Command& cmd);

#ifdef JFIX_ENABLE_MOTOR
    static void handleMotor(const Command& cmd);
#endif

#ifdef JFIX_ENABLE_GRAPHICS
    static void handleDeleteEvents(const Command& cmd);
    static void handleSync(const Command& cmd);
    static void handleAddEvent(const Command& cmd);
    static void handleAddEnv(const Command& cmd);
    static void handleSetVal(const Command& cmd);
    static void handleSetValN(const Command& cmd);
    static void handleSetCustomArg(const Command& cmd);
    static void handleLinkBus(const Command& cmd);
    static void handleSetParamBus(const Command& cmd);
#endif

#ifndef JFIX_EMULATION
    QueueHandle_t cmdQueue;
#endif
};

#endif // PARSER_H
