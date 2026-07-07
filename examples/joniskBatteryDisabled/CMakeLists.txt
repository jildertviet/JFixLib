cmake_minimum_required(VERSION 3.16)

# Same as the jonisk example (I2C sensors + OTA + UART) with ONE addition:
# JFIX_DISABLE_BATTERY. That flag is the whole point of this example — it makes
# the unit run only on the 12V input and power off when the input is removed,
# instead of running from the battery. See README.md for the details.
set(JFIX_ENABLE_OTA         ON CACHE BOOL "" FORCE)
set(JFIX_ENABLE_I2C_SENSORS ON CACHE BOOL "" FORCE)
set(JFIX_ENABLE_UART        ON CACHE BOOL "" FORCE)
set(JFIX_DISABLE_BATTERY    ON CACHE BOOL "" FORCE)

set(EXTRA_COMPONENT_DIRS ../../JFixLib ../../nanopb src)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(joniskBatteryDisabled)
