#ifndef JFIX_PLATFORM_H
#define JFIX_PLATFORM_H

// Platform abstraction for JFixLib.
// When JFIX_EMULATION is defined, ESP32-specific APIs are replaced with
// portable stubs so the library can compile on desktop (e.g. openFrameworks).

#ifdef JFIX_EMULATION

// ── Timing ──────────────────────────────────────────────────────────────────
#include <chrono>
#include <cstdint>

static inline int64_t esp_timer_get_time() {
    using namespace std::chrono;
    return duration_cast<microseconds>(
        steady_clock::now().time_since_epoch()).count();
}

// ── Logging ─────────────────────────────────────────────────────────────────
#include <cstdio>

#define ESP_LOGI(tag, fmt, ...) printf("[I][%s] " fmt "\n", tag, ##__VA_ARGS__)
#define ESP_LOGW(tag, fmt, ...) printf("[W][%s] " fmt "\n", tag, ##__VA_ARGS__)
#define ESP_LOGE(tag, fmt, ...) printf("[E][%s] " fmt "\n", tag, ##__VA_ARGS__)

// ── Error types ─────────────────────────────────────────────────────────────
typedef int esp_err_t;
#define ESP_OK              0
#define ESP_FAIL            (-1)
#define ESP_ERR_NO_MEM      0x101
#define ESP_ERROR_CHECK(x)  do { (void)(x); } while(0)

static inline const char* esp_err_to_name(esp_err_t err) {
    return (err == ESP_OK) ? "ESP_OK" : "ESP_FAIL";
}

// ── Random ──────────────────────────────────────────────────────────────────
#include <cstdlib>

static inline uint32_t esp_random() {
    return (uint32_t)rand();
}

// ── FreeRTOS stubs ──────────────────────────────────────────────────────────
#define vTaskDelay(x)       do { } while(0)
#define pdMS_TO_TICKS(x)    (x)
#define portTICK_PERIOD_MS  1
#define portMAX_DELAY       0xFFFFFFFF
#define pdTRUE              1
#define pdFALSE             0

// ── GPIO stubs ──────────────────────────────────────────────────────────────
typedef int gpio_num_t;
#define GPIO_NUM_4   4
#define GPIO_NUM_5   5
#define GPIO_NUM_12  12
#define GPIO_NUM_13  13
#define GPIO_NUM_16  16
#define GPIO_NUM_40  40
#define GPIO_NUM_41  41
#define GPIO_MODE_OUTPUT 0

static inline esp_err_t gpio_set_direction(gpio_num_t pin, int mode) { return ESP_OK; }
static inline esp_err_t gpio_set_level(gpio_num_t pin, int level) { return ESP_OK; }

// ── LEDC (PWM) stubs ───────────────────────────────────────────────────────
typedef int ledc_mode_t;
typedef int ledc_timer_t;
typedef int ledc_timer_bit_t;
typedef int ledc_channel_t;

#define LEDC_LOW_SPEED_MODE  0
#define LEDC_TIMER_0         0
#define LEDC_TIMER_12_BIT    12
#define LEDC_TIMER_13_BIT    13
#define LEDC_CHANNEL_0       0
#define LEDC_INTR_DISABLE    0
#define LEDC_AUTO_CLK        0

struct ledc_timer_config_t {
    int speed_mode; int duty_resolution; int timer_num;
    uint32_t freq_hz; int clk_cfg;
};
struct ledc_channel_config_t {
    int gpio_num; int speed_mode; int channel; int intr_type;
    int timer_sel; uint32_t duty; int hpoint;
    struct { int output_invert; } flags;
};

static inline esp_err_t ledc_timer_config(const ledc_timer_config_t*) { return ESP_OK; }
static inline esp_err_t ledc_channel_config(const ledc_channel_config_t*) { return ESP_OK; }
static inline esp_err_t ledc_set_duty(int, int, uint32_t) { return ESP_OK; }
static inline esp_err_t ledc_update_duty(int, int) { return ESP_OK; }

// ── LED strip stubs ─────────────────────────────────────────────────────────
typedef void* led_strip_handle_t;

#define LED_MODEL_WS2812                    0
#define LED_STRIP_COLOR_COMPONENT_FMT_GRB   0
#define RMT_CLK_SRC_DEFAULT                 0

struct led_strip_config_t {
    int strip_gpio_num; int max_leds; int led_model;
    int color_component_format;
    struct { bool invert_out; } flags;
};
struct led_strip_rmt_config_t {
    int clk_src; uint32_t resolution_hz; int mem_block_symbols;
    struct { bool with_dma; } flags;
};

static inline esp_err_t led_strip_new_rmt_device(const led_strip_config_t*, const led_strip_rmt_config_t*, led_strip_handle_t*) { return ESP_OK; }
static inline esp_err_t led_strip_set_pixel(led_strip_handle_t, int, uint32_t, uint32_t, uint32_t) { return ESP_OK; }
static inline esp_err_t led_strip_refresh(led_strip_handle_t) { return ESP_OK; }
static inline esp_err_t led_strip_del(led_strip_handle_t) { return ESP_OK; }

// ── Sleep / System stubs ────────────────────────────────────────────────────
static inline void esp_sleep_enable_timer_wakeup(uint64_t) {}
static inline void esp_deep_sleep_start() {}
static inline void esp_restart() {}

// ── NVS stubs ───────────────────────────────────────────────────────────────
// (NVSStorage emulation implementation provided in NVSStorage.cpp)

// ── WiFi / ESP-NOW stubs ────────────────────────────────────────────────────
// Not needed — entire WiFi/ESPNOW code is #ifdef'd out in emulation mode.

#else // !JFIX_EMULATION — real ESP32 headers

#include "esp_timer.h"
#include "esp_log.h"

#endif // JFIX_EMULATION

#endif // JFIX_PLATFORM_H
