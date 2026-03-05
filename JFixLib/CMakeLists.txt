# ── Feature flags (set by each example's root CMakeLists.txt) ─────────────────
# ESP-IDF processes component CMakeLists before project-level set(CACHE FORCE),
# so option() defaults apply on the first configure pass. The REQUIRES list is
# kept static (all IDF deps always present) — only SRCS are conditional.
# The linker's --gc-sections strips unused code from unreferenced components.
option(JFIX_ENABLE_GRAPHICS    "Event system, Perlin, Rect, Osc, Env, Wavetable" OFF)
option(JFIX_ENABLE_OTA         "OTA updater (HTTP)"                              OFF)
option(JFIX_ENABLE_ETHERNET    "W5500 SPI Ethernet handler"                      OFF)
option(JFIX_ENABLE_I2C_SENSORS "BQ25792, MMA8451, I2CWrapper"                    OFF)
option(JFIX_ENABLE_MOTOR       "Stepper motor controller"                        OFF)
option(JFIX_ENABLE_UART        "UART handler"                                    OFF)

# ── Core sources (always compiled) ───────────────────────────────────────────
set(JFIX_SRCS
    jfixture.cpp
    parser.cpp
    blink.cpp
    NVSStorage.cpp
    dimmer.cpp
    lagger.cpp
    espnow_handler.cpp
    generated/simple.pb.c
)

# ── Optional: Graphics ───────────────────────────────────────────────────────
if(JFIX_ENABLE_GRAPHICS)
    list(APPEND JFIX_SRCS
        jfixture_addr.cpp
        jfixture_graphics.cpp
        Event.cpp
        JEnv.cpp
        JOsc.cpp
        JWavetable.cpp
        JEvent_Perlin.cpp
        JRect.cpp
        expRandLines.cpp
    )
endif()

# ── Optional: OTA ────────────────────────────────────────────────────────────
if(JFIX_ENABLE_OTA)
    list(APPEND JFIX_SRCS OTAUpdater.cpp)
endif()

# ── Optional: Ethernet ───────────────────────────────────────────────────────
if(JFIX_ENABLE_ETHERNET)
    list(APPEND JFIX_SRCS ethernet_handler.cpp)
endif()

# ── Optional: I2C sensors ────────────────────────────────────────────────────
if(JFIX_ENABLE_I2C_SENSORS)
    list(APPEND JFIX_SRCS I2CWrapper.cpp BQ25792.cpp MMA8451.cpp jonisk.cpp)
endif()

# ── Optional: Motor ──────────────────────────────────────────────────────────
if(JFIX_ENABLE_MOTOR)
    list(APPEND JFIX_SRCS motor_controller.cpp)
endif()

# ── Optional: UART ───────────────────────────────────────────────────────────
if(JFIX_ENABLE_UART)
    list(APPEND JFIX_SRCS UART.cpp)
endif()

# ── Register component ──────────────────────────────────────────────────────
# All IDF dependencies listed statically so component registration works
# regardless of option() evaluation order in ESP-IDF's build system.
idf_component_register(
    SRCS ${JFIX_SRCS}
    INCLUDE_DIRS . generated
    REQUIRES esp_driver_gpio nvs_flash esp_http_client esp_https_ota
             esp_wifi esp_event esp_netif freertos esp_driver_i2c
             esp_driver_ledc esp_driver_uart esp_timer nanopb led_strip
             esp_eth esp_driver_spi esp_system esp_hw_support
)

# ── Propagate flags as compile definitions ───────────────────────────────────
if(JFIX_ENABLE_GRAPHICS)
    target_compile_definitions(${COMPONENT_LIB} PUBLIC JFIX_ENABLE_GRAPHICS)
endif()
if(JFIX_ENABLE_OTA)
    target_compile_definitions(${COMPONENT_LIB} PUBLIC JFIX_ENABLE_OTA)
endif()
if(JFIX_ENABLE_ETHERNET)
    target_compile_definitions(${COMPONENT_LIB} PUBLIC JFIX_ENABLE_ETHERNET)
endif()
if(JFIX_ENABLE_I2C_SENSORS)
    target_compile_definitions(${COMPONENT_LIB} PUBLIC JFIX_ENABLE_I2C_SENSORS)
endif()
if(JFIX_ENABLE_MOTOR)
    target_compile_definitions(${COMPONENT_LIB} PUBLIC JFIX_ENABLE_MOTOR)
endif()
if(JFIX_ENABLE_UART)
    target_compile_definitions(${COMPONENT_LIB} PUBLIC JFIX_ENABLE_UART)
endif()
