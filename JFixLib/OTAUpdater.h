#ifndef OTA_UPDATER_H
#define OTA_UPDATER_H

#ifndef JFIX_EMULATION

#include "esp_err.h"

class OTAUpdater {
public:
  OTAUpdater();
  void checkForOTA();
};
extern OTAUpdater ota;

#endif // !JFIX_EMULATION

#endif // OTA_UPDATER_H
