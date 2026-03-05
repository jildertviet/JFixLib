#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include <stdio.h>

// Define GPIOs
#define BLINK_GPIO GPIO_NUM_5
#define I2C_SDA_PIN GPIO_NUM_12
#define I2C_SCL_PIN GPIO_NUM_13
#ifdef CONFIG_IDF_TARGET_ESP32S3
#define BQ_CE_PIN  GPIO_NUM_41
#define BQ_INT_PIN GPIO_NUM_40
#else
// ESP32 classic only has GPIOs 0-39; BQ25792 not used on this target
#define BQ_CE_PIN  GPIO_NUM_4
#define BQ_INT_PIN GPIO_NUM_5
#endif
