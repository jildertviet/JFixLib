#include "NVSStorage.h"
#include "nvs_flash.h"
#include "nvs.h"
#include <vector>

NVSStorage::NVSStorage(const char* nspace)
    : nvs_namespace(nspace) {
}

esp_err_t NVSStorage::writeString(const char* key, const std::string& value) {
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open(nvs_namespace, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        return err;
    }

    err = nvs_set_str(my_handle, key, value.c_str());
    if (err == ESP_OK) {
        err = nvs_commit(my_handle);
    }

    nvs_close(my_handle);
    return err;
}

esp_err_t NVSStorage::readString(const char* key, std::string& out_value) {
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open(nvs_namespace, NVS_READONLY, &my_handle);
    if (err != ESP_OK) {
        return err;
    }

    size_t required_size = 0;
    err = nvs_get_str(my_handle, key, NULL, &required_size);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        nvs_close(my_handle);
        return err;
    }

    if (required_size > 0) {
        std::vector<char> buf(required_size);
        err = nvs_get_str(my_handle, key, buf.data(), &required_size);
        if (err == ESP_OK) {
            out_value = std::string(buf.data());
        }
    } else {
        out_value = "";
    }

    nvs_close(my_handle);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        return ESP_OK;
    }

    return err;
}