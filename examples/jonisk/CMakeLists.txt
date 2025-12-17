# The following lines of code are for the "jonisk" project.
# This project is designed for the ESP-IDF framework and targets the esp32s3.
# It defines the project name and includes the necessary components.

cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS ../../JFixLib src)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(jonisk)
