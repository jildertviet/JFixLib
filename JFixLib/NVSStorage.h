#ifndef NVS_STORAGE_H
#define NVS_STORAGE_H

#include "esp_err.h"
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
