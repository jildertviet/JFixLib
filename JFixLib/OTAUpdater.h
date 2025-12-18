#ifndef OTA_UPDATER_H
#define OTA_UPDATER_H

#include "esp_err.h"

class OTAUpdater {
public:
  OTAUpdater();
  void checkForOTA();
};

#endif // OTA_UPDATER_H
