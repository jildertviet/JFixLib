cmake_minimum_required(VERSION 3.16)

# JFixLib feature flags — Jonisk uses I2C sensors + OTA + UART
set(JFIX_ENABLE_OTA         ON CACHE BOOL "" FORCE)
set(JFIX_ENABLE_I2C_SENSORS ON CACHE BOOL "" FORCE)
set(JFIX_ENABLE_UART        ON CACHE BOOL "" FORCE)

set(EXTRA_COMPONENT_DIRS ../../JFixLib ../../nanopb src)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(jonisk)
