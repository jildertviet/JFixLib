#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>
#include <stddef.h>
#include <map>
#include "generated/simple.pb.h"

class Parser {
public:
    static Parser& getInstance();

    // Parse binary ProtoBuf commands (e.g. from ESP-NOW or UART binary mode)
    void processIncomingBuffer(uint8_t* buffer, size_t size);

private:
    Parser();
    
    typedef void (*HandlerFunc)(const Command& cmd);
    std::map<pb_size_t, HandlerFunc> dispatcher;

    static void handleLed(const Command& cmd);
    static void handleChannel(const Command& cmd);
    static void handleWifi(const Command& cmd);
    static void handleId(const Command& cmd);
    static void handleMotor(const Command& cmd);
    static void handleBlink(const Command& cmd);
};

#endif // PARSER_H
