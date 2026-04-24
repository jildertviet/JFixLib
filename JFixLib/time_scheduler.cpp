#include "time_scheduler.h"
#include "jfixture.h"
#include <sys/time.h>
#include <time.h>

#ifndef JFIX_EMULATION
#include "esp_log.h"
#include "esp_netif_sntp.h"
#include "esp_sntp.h"
#include "esp_system.h"
#endif

static const char *TAG = "JTimeScheduler";

// Chunk size for LIGHT_SLEEP: wake every 5 min to re-evaluate schedule.
// Shorter = more responsive to config changes, longer = less wake overhead.
static constexpr uint32_t LIGHT_SLEEP_CHUNK_SEC = 300;

JTimeScheduler &JTimeScheduler::getInstance() {
  static JTimeScheduler instance;
  return instance;
}

void JTimeScheduler::setTimezone(const char *tz) { tzString = tz; }
void JTimeScheduler::setNtpServer(const char *server) { ntpServer = server; }

void JTimeScheduler::addSlot(uint8_t dayMask, uint16_t startMin, uint16_t endMin,
                             Action action) {
  if (numSlots >= MAX_SLOTS) {
#ifndef JFIX_EMULATION
    ESP_LOGW(TAG, "Slot capacity reached (%d), ignoring", MAX_SLOTS);
#endif
    return;
  }
  slots[numSlots++] = Slot{dayMask, startMin, endMin, action, false};
}

void JTimeScheduler::onGotIp() {
#ifndef JFIX_EMULATION
  ESP_LOGI(TAG, "SNTP sync starting (server=%s)", ntpServer);
  esp_sntp_config_t cfg = ESP_NETIF_SNTP_DEFAULT_CONFIG(ntpServer);
  esp_err_t err = esp_netif_sntp_init(&cfg);
  if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
    ESP_LOGE(TAG, "esp_netif_sntp_init failed: %d", err);
    return;
  }
  err = esp_netif_sntp_sync_wait(pdMS_TO_TICKS(5000));
  if (err != ESP_OK) {
    ESP_LOGW(TAG, "SNTP sync timed out — scheduler will stay inert");
    return;
  }
  setenv("TZ", tzString, 1);
  tzset();
  bSynced = true;

  time_t now = 0;
  time(&now);
  struct tm ti;
  localtime_r(&now, &ti);
  ESP_LOGI(TAG, "SNTP synced: %04d-%02d-%02d %02d:%02d:%02d (TZ=%s)",
           ti.tm_year + 1900, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour, ti.tm_min,
           ti.tm_sec, tzString);
#endif
}

bool JTimeScheduler::isSlotActive(const Slot &s, int dow,
                                  uint16_t minute) const {
  uint8_t prevDow = (uint8_t)((dow + 6) % 7);
  if (s.endMin > s.startMin) {
    return (s.dayMask & (1 << dow)) && minute >= s.startMin &&
           minute < s.endMin;
  }
  // Wraps past midnight.
  bool todayPart = (s.dayMask & (1 << dow)) && minute >= s.startMin;
  bool yestPart = (s.dayMask & (1 << prevDow)) && minute < s.endMin;
  return todayPart || yestPart;
}

uint32_t JTimeScheduler::remainingSlotSeconds(const Slot &s, int dow,
                                              uint16_t minute, int sec) const {
  uint32_t curSec = (uint32_t)minute * 60 + (uint32_t)sec;
  uint32_t endSec = (uint32_t)s.endMin * 60;
  if (s.endMin > s.startMin) {
    return endSec > curSec ? endSec - curSec : 0;
  }
  // Wraps: if we are in the start-day part, add 24 h before endSec.
  if (minute >= s.startMin) {
    return 24u * 3600u - curSec + endSec;
  }
  return endSec > curSec ? endSec - curSec : 0;
}

void JTimeScheduler::tick() {
#ifndef JFIX_EMULATION
  if (!bSynced || numSlots == 0 || !jFixture::instance) {
    return;
  }

  time_t now;
  time(&now);
  struct tm ti;
  localtime_r(&now, &ti);
  int dow = ti.tm_wday;
  uint16_t minute = (uint16_t)(ti.tm_hour * 60 + ti.tm_min);

  bool idleRequested = false;
  uint64_t sleepUs = 0;

  for (int i = 0; i < numSlots; i++) {
    Slot &s = slots[i];
    bool active = isSlotActive(s, dow, minute);

    if (active) {
      if (s.action == IDLE) {
        idleRequested = true;
      } else if (s.action == LIGHT_SLEEP) {
        idleRequested = true;
        uint32_t remaining = remainingSlotSeconds(s, dow, minute, ti.tm_sec);
        uint32_t chunk =
            remaining < LIGHT_SLEEP_CHUNK_SEC ? remaining : LIGHT_SLEEP_CHUNK_SEC;
        if (chunk > 0) {
          sleepUs = (uint64_t)chunk * 1000000ULL;
        }
      } else if (s.action == REBOOT && !s.wasActive) {
        ESP_LOGI(TAG, "Slot %d entered (REBOOT) — restarting", i);
        esp_restart();
      }
    }
    s.wasActive = active;
  }

  jFixture::instance->bIdle = idleRequested;
  jFixture::instance->nextLightSleepUs = sleepUs;
#endif
}
