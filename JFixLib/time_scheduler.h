#pragma once
#include <stdint.h>

// JTimeScheduler — SNTP-synced time-of-day scheduler.
//
// The device gets one shot at SNTP sync during the brief WiFi-connected
// window at boot (right before the OTA check). After that the RTC free-runs.
// A REBOOT slot once per night is the recommended way to re-sync long-running
// devices.
//
// Slots that cross midnight (endMinuteOfDay <= startMinuteOfDay) are
// supported: the slot is in effect on the start-day night and continues into
// the following morning.
//
// Requires JFIX_ENABLE_SCHEDULER. Without a WiFi uplink at boot the scheduler
// stays inert (hasSynced() == false) and all slots are ignored — fail-safe
// behaviour is "lights stay on".
class JTimeScheduler {
public:
  enum Action {
    IDLE,        // sets jFixture::bIdle — writeLeds writes black
    LIGHT_SLEEP, // IDLE + esp_light_sleep_start in 300 s chunks (ESP-NOW unreachable)
    REBOOT       // one-shot esp_restart on slot entry (use for nightly re-sync)
  };

  // Day mask bits match tm_wday (0 = Sunday).
  static constexpr uint8_t DAYS_SUN = 1 << 0;
  static constexpr uint8_t DAYS_MON = 1 << 1;
  static constexpr uint8_t DAYS_TUE = 1 << 2;
  static constexpr uint8_t DAYS_WED = 1 << 3;
  static constexpr uint8_t DAYS_THU = 1 << 4;
  static constexpr uint8_t DAYS_FRI = 1 << 5;
  static constexpr uint8_t DAYS_SAT = 1 << 6;
  static constexpr uint8_t DAYS_ALL = 0x7F;
  static constexpr uint8_t DAYS_WEEKDAYS =
      DAYS_MON | DAYS_TUE | DAYS_WED | DAYS_THU | DAYS_FRI;
  static constexpr uint8_t DAYS_WEEKEND = DAYS_SAT | DAYS_SUN;

  static constexpr uint16_t hhmm(uint8_t h, uint8_t m) { return h * 60 + m; }

  static JTimeScheduler &getInstance();

  // Register a slot. Safe to call before SNTP sync; slot is inert until synced.
  void addSlot(uint8_t dayMask, uint16_t startMinuteOfDay,
               uint16_t endMinuteOfDay, Action action);

  // Override TZ. Default: "CET-1CEST,M3.5.0,M10.5.0/3" (Europe/Amsterdam).
  void setTimezone(const char *tz);

  // Override NTP server. Default: "pool.ntp.org".
  void setNtpServer(const char *server);

  // Called from jFixture::connectWiFi() after GOT_IP. Blocks up to 30 s while
  // probing 3 NTP servers (one is a numeric IP to bypass DNS).
  void onGotIp();

  // Called every loop from jFixture::update().
  void tick();

  bool hasSynced() const { return bSynced; }

private:
  JTimeScheduler() = default;

  struct Slot {
    uint8_t dayMask;
    uint16_t startMin;
    uint16_t endMin;
    Action action;
    bool wasActive;
  };

  bool isSlotActive(const Slot &s, int dow, uint16_t minute) const;
  uint32_t remainingSlotSeconds(const Slot &s, int dow, uint16_t minute,
                                int sec) const;

  static constexpr int MAX_SLOTS = 8;
  Slot slots[MAX_SLOTS] = {};
  int numSlots = 0;

  const char *tzString = "CET-1CEST,M3.5.0,M10.5.0/3";
  const char *ntpServer = "pool.ntp.org";
  bool bSynced = false;
};
