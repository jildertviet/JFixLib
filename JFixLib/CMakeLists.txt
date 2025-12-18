idf_component_register(
    SRCS "jonisk.cpp"
    "blink.cpp"
    "NVSStorage.cpp"
    "jfixture.cpp"
    INCLUDE_DIRS .
    REQUIRES esp_driver_gpio mbedtls nvs_flash
)
