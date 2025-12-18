#ifndef NVS_STORAGE_H
#define NVS_STORAGE_H

#include "esp_err.h"
#include <string>

class NVSStorage {
public:
  NVSStorage(const char *nspace = "storage");

  esp_err_t readString(const char *key, std::string &out_value);
  esp_err_t writeString(const char *key, const std::string &value);

  // Future methods can be added here:
  // esp_err_t readBool(const char* key, bool& out_value);
  // esp_err_t writeBool(const char* key, bool value);

private:
  const char *nvs_namespace;
};

#endif // NVS_STORAGE_H
