#ifndef NVS_STORAGE_H
#define NVS_STORAGE_H

#include "jfix_platform.h"
#ifndef JFIX_EMULATION
#include "esp_err.h"
#endif
#include <string>

class NVSStorage {
public:
  NVSStorage(const char *nspace = "storage");
  esp_err_t init();

  esp_err_t readString(const char *key, std::string &outValue);
  esp_err_t writeString(const char *key, const std::string &value);

  bool bInit = false;

private:
  const char *nvsNamespace;
};

extern NVSStorage nvs;
#endif // NVS_STORAGE_H
