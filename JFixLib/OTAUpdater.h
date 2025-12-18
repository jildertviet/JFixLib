#ifndef OTA_UPDATER_H
#define OTA_UPDATER_H

#include "esp_err.h"

class OTAUpdater {
public:
  OTAUpdater();
  void checkForOTA();
};
extern OTAUpdater ota;

#endif // OTA_UPDATER_H
