#include "jfix_platform.h"
#include <stdio.h>

#ifndef JFIX_EMULATION
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#endif

// Define GPIOs
#define BLINK_GPIO GPIO_NUM_5
#define I2C_SDA_PIN GPIO_NUM_12
#define I2C_SCL_PIN GPIO_NUM_13
#ifndef JFIX_EMULATION
#ifdef CONFIG_IDF_TARGET_ESP32S3
#define BQ_CE_PIN  GPIO_NUM_41
#define BQ_INT_PIN GPIO_NUM_40
#else
#define BQ_CE_PIN  GPIO_NUM_4
#define BQ_INT_PIN GPIO_NUM_5
#endif
#else
#define BQ_CE_PIN  4
#define BQ_INT_PIN 5
#endif
