cmake_minimum_required(VERSION 3.16)

# JFixLib feature flags
set(JFIX_ENABLE_GRAPHICS ON CACHE BOOL "" FORCE)
set(JFIX_ENABLE_OTA      ON CACHE BOOL "" FORCE)
set(JFIX_ENABLE_ETHERNET ON CACHE BOOL "" FORCE)
set(JFIX_ENABLE_UART     ON CACHE BOOL "" FORCE)

set(EXTRA_COMPONENT_DIRS ../../JFixLib ../../nanopb src)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(tlFixEthernet)
