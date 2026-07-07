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

#include "JOsc.h"
#include "esp_random.h"
#include "time_scheduler.h"

static void addBootEvent() {
  // viewport[0] = 20, string read positions are at x=0 and
  // x=horizontalPixelDistance (10). Constrain each oscillator to its own string
  // by clipping size_x.
  const float locX[2] = {0.0f, 0.5f}; // 0 and 10 in viewport units
  const float sizeX = 0.25f; // 5 viewport units — covers one string only
  const float colors[2][3] = {
      {1.0f, 0.0f, 0.0f}, // left  (string 0) — red
      {0.0f, 0.0f, 1.0f}, // right (string 1) — blue
  };
  for (int i = 0; i < 2; i++) {
    JOsc *j = new JOsc(fix.getWavetable());
    j->id = 1 + i;
    // ±20% jitter around 0.1 Hz
    float jitter = ((float)esp_random() / (float)UINT32_MAX) * 0.04f - 0.02f;
    j->frequency = 0.1f + jitter;
    j->range = 1.0f;
    j->loc[0] = locX[i];
    j->size[0] = sizeX;

    j->rgba[0] = colors[i][0];
    j->rgba[1] = colors[i][1];
    j->rgba[2] = colors[i][2];
    j->rgba[3] = 1.0f;
    j->start();
    fix.addEvent(j);
  }
}

extern "C" void app_main(void) {
  fix.init();

  JTlFixSettings s;
  s.pins = kPins;
#ifdef USE_ETHERNET
  fix.setup(s, kDeviceId);
#else
  fix.setup(s);
#endif

  // Reshape the shared sine wavetable: power < 1 brightens (more light, less
  // black); default is 10.0f (sharp bright peak).
  fix.getWavetable()->fillSineNorm(0.3f);

  addBootEvent();

  // Time-of-day scheduler. SNTP syncs once during boot (before WiFi
  // disconnects for ESP-NOW). After sync, slots gate the LED output.
  // Uncomment and tune for the installation's operating hours.

  auto &sched = JTimeScheduler::getInstance();
  sched.addSlot(JTimeScheduler::DAYS_ALL, JTimeScheduler::hhmm(23, 0),
                JTimeScheduler::hhmm(7, 1), JTimeScheduler::IDLE);
  sched.addSlot(JTimeScheduler::DAYS_ALL, JTimeScheduler::hhmm(7, 1),
                JTimeScheduler::hhmm(7, 2), JTimeScheduler::REBOOT);
  while (1) {
    fix.update();
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}
