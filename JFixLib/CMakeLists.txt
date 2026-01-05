idf_component_register(
    SRCS "jonisk.cpp"
    "blink.cpp"
    "NVSStorage.cpp"
    "jfixture.cpp"
    "OTAUpdater.cpp"
    "I2CWrapper.cpp"
    "BQ25792.cpp"
    INCLUDE_DIRS .
    REQUIRES esp_driver_gpio mbedtls nvs_flash esp_http_client esp_https_ota esp_wifi esp_event esp_netif freertos esp_driver_i2c esp_driver_ledc
)
