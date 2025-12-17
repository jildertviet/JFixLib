idf_component_register(
    SRCS "jonisk.cpp"
    "blink.cpp"
    INCLUDE_DIRS .
    REQUIRES esp_driver_gpio mbedtls
)
