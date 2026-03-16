#include "parser.h"
#include "jfix_platform.h"
#ifndef JFIX_EMULATION
#include "esp_sleep.h"
#include "esp_system.h"
#endif
#include "NVSStorage.h"
#include "dimmer.h"
#include "jfixture.h"
#include "blink.h"
#include <pb_decode.h>
#include <string.h>

#ifdef JFIX_ENABLE_GRAPHICS
#include "jfixture_graphics.h"
#include "JEvent_Perlin.h"
#include "JRect.h"
#include "JOsc.h"
#endif

#ifdef JFIX_ENABLE_MOTOR
#include "motor_controller.h"
#endif

static const char *TAG = "Parser";

Parser::Parser() {
    // Simple control
    dispatcher[Command_led_tag]           = handleLed;
    dispatcher[Command_channel_tag]       = handleChannel;
    dispatcher[Command_wifi_tag]          = handleWifi;
    dispatcher[Command_set_id_tag]        = handleId;
    dispatcher[Command_blink_tag]         = handleBlink;
    dispatcher[Command_sleep_tag]         = handleSleep;
    dispatcher[Command_lag_tag]           = handleLag;
    dispatcher[Command_reboot_tag]        = handleReboot;
    dispatcher[Command_set_ota_url_tag]          = handleSetOtaUrl;
    dispatcher[Command_set_background_tag]       = handleSetBackground;
    dispatcher[Command_set_viewport_offset_tag]  = handleSetViewportOffset;

#ifdef JFIX_ENABLE_MOTOR
    dispatcher[Command_motor_tag]         = handleMotor;
#endif

#ifdef JFIX_ENABLE_GRAPHICS
    dispatcher[Command_delete_events_tag] = handleDeleteEvents;
    dispatcher[Command_sync_tag]          = handleSync;
    dispatcher[Command_add_event_tag]     = handleAddEvent;
    dispatcher[Command_add_env_tag]       = handleAddEnv;
    dispatcher[Command_set_val_tag]       = handleSetVal;
    dispatcher[Command_set_val_n_tag]     = handleSetValN;
    dispatcher[Command_set_custom_tag]    = handleSetCustomArg;
    dispatcher[Command_link_bus_tag]      = handleLinkBus;
    dispatcher[Command_set_param_bus_tag] = handleSetParamBus;
#endif
}

Parser& Parser::getInstance() {
    static Parser instance;
    return instance;
}

void Parser::processIncomingBuffer(uint8_t* buffer, size_t size) {
    size_t offset = 0;
    while (offset + 2 <= size) {
        uint16_t cmdLen = (buffer[offset] << 8) | buffer[offset + 1];
        offset += 2;
        if (cmdLen == 0 || offset + cmdLen > size) {
            ESP_LOGE(TAG, "Invalid command length %d at offset %d", (int)cmdLen, (int)(offset - 2));
            break;
        }

        Command cmd = Command_init_default;
        pb_istream_t stream = pb_istream_from_buffer(buffer + offset, cmdLen);
        if (pb_decode(&stream, Command_fields, &cmd)) {
            dispatchCommand(cmd);
        } else {
            ESP_LOGE(TAG, "ProtoBuf decoding failed: %s", PB_GET_ERROR(&stream));
        }
        offset += cmdLen;
    }
}

void Parser::dispatchCommand(const Command& cmd) {
    int myId = (jFixture::instance) ? jFixture::instance->getId() : 0;
    if (cmd.id != 255 && cmd.id != myId) {
        return;
    }

    if (dispatcher.count(cmd.which_payload)) {
        dispatcher[cmd.which_payload](cmd);
    } else {
        ESP_LOGW(TAG, "Unknown command tag: %d", (int)cmd.which_payload);
    }
}

// ── Simple control ──────────────────────────────────────────────────────────

void Parser::handleLed(const Command& cmd) {
    float val = cmd.payload.led.brightness;
    if (jFixture::instance) {
        jFixture::instance->setBrightness(val);
        ESP_LOGI(TAG, "Brightness (lagged) → %.2f", val);
    } else {
        dimmer.setBrightness(val);
        dimmer.show();
        ESP_LOGI(TAG, "Brightness (immediate) → %.2f", val);
    }
}

void Parser::handleChannel(const Command& cmd) {
    int ch = cmd.payload.channel.channel;
    float val = cmd.payload.channel.value;
    dimmer.setChannel(ch, val);
    dimmer.show();
    ESP_LOGI(TAG, "Channel %d → %.2f", ch, val);
}

void Parser::handleWifi(const Command& cmd) {
    const char* ssid = cmd.payload.wifi.ssid;
    const char* pass = cmd.payload.wifi.password;
    nvs.writeString("ssid", ssid);
    nvs.writeString("password", pass);
    if (strlen(pass) > 0) {
        ESP_LOGI(TAG, "WiFi creds updated: %s (restart to apply)", ssid);
    } else {
        ESP_LOGI(TAG, "WiFi SSID updated (open): %s (restart to apply)", ssid);
    }
}

void Parser::handleId(const Command& cmd) {
    if (!jFixture::instance) return;
    const IdCmd& c = cmd.payload.set_id;
    if (!jFixture::instance->macMatches(c.mac.bytes, c.mac.size)) {
        ESP_LOGW(TAG, "setId: MAC mismatch, ignoring");
        return;
    }
    jFixture::instance->setId((int)c.id);
}

#ifdef JFIX_ENABLE_MOTOR
void Parser::handleMotor(const Command& cmd) {
    int32_t steps = cmd.payload.motor.steps;
    float speed   = cmd.payload.motor.speed;
    bool relative = cmd.payload.motor.relative;
    if (relative) {
        motorController.move(steps, speed);
        ESP_LOGI(TAG, "Motor rel %d @ %.2f", (int)steps, speed);
    } else {
        motorController.moveTo(steps, speed);
        ESP_LOGI(TAG, "Motor abs %d @ %.2f", (int)steps, speed);
    }
}
#endif

void Parser::handleBlink(const Command& cmd) {
    uint16_t on  = (uint16_t)cmd.payload.blink.on_ms;
    uint16_t off = (uint16_t)cmd.payload.blink.off_ms;
    blink.setInterval(on, off);
    ESP_LOGI(TAG, "Blink ON=%d OFF=%d ms", on, off);
}

void Parser::handleSleep(const Command& cmd) {
    int32_t ms = cmd.payload.sleep.duration_ms;
    ESP_LOGI(TAG, "Deep sleep %d ms (ignored in emulation)", (int)ms);
#ifndef JFIX_EMULATION
    if (ms > 0) {
        esp_sleep_enable_timer_wakeup((uint64_t)ms * 1000ULL);
    }
    esp_deep_sleep_start();
#endif
}

void Parser::handleLag(const Command& cmd) {
    int lagger_id = cmd.payload.lag.lagger_id;
    float lag_ms  = cmd.payload.lag.lag_time_ms;
    if (jFixture::instance) {
        jFixture::instance->setLagTime(lagger_id, lag_ms);
        ESP_LOGI(TAG, "Lagger %d → %.2f ms", lagger_id, lag_ms);
    }
}

void Parser::handleReboot(const Command& cmd) {
    ESP_LOGI(TAG, "Reboot command received (ignored in emulation)");
#ifndef JFIX_EMULATION
    esp_restart();
#endif
}

void Parser::handleSetOtaUrl(const Command& cmd) {
    const char* url = cmd.payload.set_ota_url.url;
    nvs.writeString("OTAurl", url);
    ESP_LOGI(TAG, "OTA URL updated: %s (restart to apply)", url);
}

void Parser::handleSetBackground(const Command& cmd) {
    if (!jFixture::instance) return;
    const SetBackgroundCmd& c = cmd.payload.set_background;
    jFixture::instance->setBackground(c.r, c.g, c.b, c.a);
    ESP_LOGI(TAG, "Background → r=%.2f g=%.2f b=%.2f a=%.2f", c.r, c.g, c.b, c.a);
}

void Parser::handleSetViewportOffset(const Command& cmd) {
    if (!jFixture::instance) return;
    const SetViewportOffsetCmd& c = cmd.payload.set_viewport_offset;
    jFixture::instance->setViewportOffset(c.x, c.y);
    ESP_LOGI(TAG, "ViewportOffset → x=%.2f y=%.2f", c.x, c.y);
}

// ── Graphics (conditional) ──────────────────────────────────────────────────

#ifdef JFIX_ENABLE_GRAPHICS

static jFixtureGraphics* gfx() {
    return jFixture::instance ? jFixture::instance->asGraphics() : nullptr;
}

void Parser::handleDeleteEvents(const Command& cmd) {
    jFixtureGraphics *g = gfx();
    if (g) {
        g->deleteEvents();
        ESP_LOGI(TAG, "All events deleted");
    } else {
        ESP_LOGW(TAG, "deleteEvents: not a jFixtureGraphics");
    }
}

void Parser::handleSync(const Command& cmd) {
    int event_id = cmd.payload.sync.event_id;
    jFixtureGraphics *g = gfx();
    if (g) {
        g->sync(event_id);
        ESP_LOGI(TAG, "Event %d synced", event_id);
    } else {
        ESP_LOGW(TAG, "sync: not a jFixtureGraphics");
    }
}

void Parser::handleAddEvent(const Command& cmd) {
    jFixtureGraphics *g = gfx();
    if (!g) { ESP_LOGW(TAG, "addEvent: not a jFixtureGraphics"); return; }

    const AddEventCmd& c = cmd.payload.add_event;
    Event *e = nullptr;

    switch (c.type) {
        case EventType_EVENT_PERLIN: {
            JEvent_Perlin *p = new JEvent_Perlin();
            p->id       = c.event_id;
            p->loc[0]   = c.loc_x;   p->loc[1]  = c.loc_y;
            p->size[0]  = c.size_x;  p->size[1] = c.size_y;
            p->rgba[0]  = c.r;  p->rgba[1] = c.g;
            p->rgba[2]  = c.b;  p->rgba[3] = c.a;
            p->brightness   = c.a;
            p->bWaitForEnv  = c.wait_for_env;
            e = p;
            break;
        }
        case EventType_EVENT_RECT: {
            JRect *r = new JRect();
            r->id       = c.event_id;
            r->loc[0]   = c.loc_x;   r->loc[1]  = c.loc_y;
            r->size[0]  = c.size_x;  r->size[1] = c.size_y;
            r->rgba[0]  = c.r;  r->rgba[1] = c.g;
            r->rgba[2]  = c.b;  r->rgba[3] = c.a;
            r->brightness   = c.a;
            r->bWaitForEnv  = c.wait_for_env;
            e = r;
            break;
        }
        case EventType_EVENT_OSC: {
            JOsc *j = new JOsc(g->getWavetable());
            j->id       = c.event_id;
            j->loc[0]   = c.loc_x;   j->loc[1]  = c.loc_y;
            j->size[0]  = c.size_x;  j->size[1] = c.size_y;
            j->rgba[0]  = c.r;  j->rgba[1] = c.g;
            j->rgba[2]  = c.b;  j->rgba[3] = c.a;
            j->brightness   = c.a;
            j->bWaitForEnv  = c.wait_for_env;
            e = j;
            break;
        }
        default:
            ESP_LOGW(TAG, "addEvent: unknown type %d", (int)c.type);
            return;
    }

    if (e) {
        g->addEvent(e);
        ESP_LOGI(TAG, "addEvent type=%d id=%d", (int)c.type, (int)c.event_id);
    }
}

void Parser::handleAddEnv(const Command& cmd) {
    jFixtureGraphics *g = gfx();
    if (!g) { ESP_LOGW(TAG, "addEnv: not a jFixtureGraphics"); return; }

    const AddEnvCmd& c = cmd.payload.add_env;
    Event *e = g->getEventByID(c.event_id);
    if (!e) {
        ESP_LOGW(TAG, "addEnv: no event with id %d", (int)c.event_id);
        return;
    }

    char v = (char)c.var;
    uint16_t a = (uint16_t)c.attack_ms;
    uint16_t s = (uint16_t)c.sustain_ms;
    uint16_t r = (uint16_t)c.release_ms;
    bool kill  = c.kill;

    switch (v) {
        case 'b': e->addEnv('b', &e->brightness, a, s, r, c.target, 0.0f, kill); break;
        case 'x': e->addEnv('x', &e->loc[0],     a, s, r, c.target, e->loc[0], kill); break;
        case 'y': e->addEnv('y', &e->loc[1],     a, s, r, c.target, e->loc[1], kill); break;
        case 'w': e->addEnv('w', &e->size[0],    a, s, r, c.target, e->size[0], kill); break;
        case 'h': e->addEnv('h', &e->size[1],    a, s, r, c.target, e->size[1], kill); break;
        default:
            ESP_LOGW(TAG, "addEnv: unknown var '%c'", v);
            return;
    }
    e->updateEnvelopes();
    e->bWaitForEnv = false;
    ESP_LOGI(TAG, "addEnv id=%d var=%c a=%d s=%d r=%d", (int)c.event_id, v, a, s, r);
}

void Parser::handleSetVal(const Command& cmd) {
    jFixtureGraphics *g = gfx();
    if (!g) { ESP_LOGW(TAG, "setVal: not a jFixtureGraphics"); return; }

    const SetValCmd& c = cmd.payload.set_val;
    Event *e = g->getEventByID(c.event_id);
    if (!e) return;

    char v = (char)c.var;
    if (v == 'c') {
        e->rgba[0] = c.r;
        e->rgba[1] = c.g;
        e->rgba[2] = c.b;
        e->rgba[3] = c.a;
    } else {
        e->setVal(v, c.value);
    }
    ESP_LOGI(TAG, "setVal id=%d var='%c'", (int)c.event_id, v);
}

void Parser::handleSetValN(const Command& cmd) {
    jFixtureGraphics *g = gfx();
    if (!g) { ESP_LOGW(TAG, "setValN: not a jFixtureGraphics"); return; }

    const SetValNCmd& c = cmd.payload.set_val_n;
    if (!jFixture::instance) return;

    int myId = jFixture::instance->getId();
    if (myId < 0 || myId >= (int)c.values_count) return;

    Event *e = g->getEventByID(c.event_id);
    if (!e) return;

    e->setVal((char)c.var, c.values[myId]);
    ESP_LOGI(TAG, "setValN id=%d var='%c' val=%.2f (slot %d)",
             (int)c.event_id, (char)c.var, c.values[myId], myId);
}

void Parser::handleSetCustomArg(const Command& cmd) {
    jFixtureGraphics *g = gfx();
    if (!g) { ESP_LOGW(TAG, "setCustomArg: not a jFixtureGraphics"); return; }

    const SetCustomArgCmd& c = cmd.payload.set_custom;
    Event *e = g->getEventByID(c.event_id);
    if (!e) return;

    e->setCustomArg((uint8_t)c.arg_id, c.value);
    ESP_LOGI(TAG, "setCustomArg id=%d arg=%d val=%.3f", (int)c.event_id, (int)c.arg_id, c.value);
}

void Parser::handleLinkBus(const Command& cmd) {
    jFixtureGraphics *g = gfx();
    if (!g) { ESP_LOGW(TAG, "linkBus: not a jFixtureGraphics"); return; }
    if (!jFixture::instance) return;

    const LinkBusCmd& c = cmd.payload.link_bus;
    Event *e = g->getEventByID(c.event_id);
    if (!e) return;

    e->linkBus((char)c.var, (char)c.bus_index, jFixture::instance->parameterBusses);
    ESP_LOGI(TAG, "linkBus id=%d var='%c' bus=%d", (int)c.event_id, (char)c.var, (int)c.bus_index);
}

void Parser::handleSetParamBus(const Command& cmd) {
    if (!jFixture::instance) return;
    const SetParamBusCmd& c = cmd.payload.set_param_bus;
    jFixture::instance->setParameterBus(c.bus_index, c.value);
    ESP_LOGI(TAG, "setParamBus [%d] = %.3f", (int)c.bus_index, c.value);
}

#endif // JFIX_ENABLE_GRAPHICS
