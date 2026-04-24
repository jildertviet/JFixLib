// Universal tlFix example.
//
// By default this builds the plain JTlFix (WiFi/ESP-NOW only).
// Uncomment USE_ETHERNET to switch to JTlFixEthernet (W5500 SPI shield).
//
// Pin notes:
//   JTlFix           strings on GPIO 22, 23
//   JTlFixEthernet   strings on GPIO 22, 27  (GPIO 23 = W5500 MOSI)
//   W5500 pins: MISO=19, MOSI=23, SCLK=18, CS=21, RST=25

// #define USE_ETHERNET

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#ifdef USE_ETHERNET
#include "tl_fix_ethernet.h"
static JTlFixEthernet fix;
static const uint8_t kPins[2] = {22, 27};
static constexpr uint8_t kDeviceId = 1; // static IP 192.168.1.{id}
#else
#include "tl_fix.h"
static JTlFix fix;
static const uint8_t kPins[2] = {22, 23};
#endif

// Uncomment to draw a full-surface white rect — useful to verify wiring
// without needing SuperCollider to send commands.
// #include "JRect.h"
// static void addTestEvent() {
//     JRect *r = new JRect();
//     r->id = 1;
//     r->bActive = true;
//     fix.addEvent(r);
// }

extern "C" void app_main(void) {
    fix.init();

    JTlFixSettings s;
    s.pins = kPins;
#ifdef USE_ETHERNET
    fix.setup(s, kDeviceId);
#else
    fix.setup(s);
#endif

    // addTestEvent();

    while (1) {
        fix.update();
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}
