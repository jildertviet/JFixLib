#ifndef JFIX_EMULATION
#include "motor_controller.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"

static const char *TAG = "MotorController";

MotorController motorController;

MotorController::MotorController() : _stepPin(GPIO_NUM_NC), _dirPin(GPIO_NUM_NC), _enPin(GPIO_NUM_NC) {}

esp_err_t MotorController::init(gpio_num_t step_pin, gpio_num_t dir_pin, gpio_num_t en_pin) {
    _stepPin = step_pin;
    _dirPin = dir_pin;
    _enPin = en_pin;

    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << _stepPin) | (1ULL << _dirPin);
    if (_enPin != GPIO_NUM_NC) {
        io_conf.pin_bit_mask |= (1ULL << _enPin);
    }
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    esp_err_t err = gpio_config(&io_conf);
    if (err != ESP_OK) return err;

    if (_enPin != GPIO_NUM_NC) {
        gpio_set_level(_enPin, 1); // Disable initially (assuming active low)
    }

    xTaskCreate(motorTask, "motor_task", 4096, this, 10, NULL);
    ESP_LOGI(TAG, "Motor initialized on pins Step:%d, Dir:%d, En:%d", _stepPin, _dirPin, _enPin);
    return ESP_OK;
}

void MotorController::move(int32_t steps, float speed) {
    _targetPos = _currentPos + steps;
    _speed = speed;
    _isMoving = true;
    _stopRequested = false;
    if (_enPin != GPIO_NUM_NC) gpio_set_level(_enPin, 0); // Enable
}

void MotorController::moveTo(int32_t position, float speed) {
    _targetPos = position;
    _speed = speed;
    _isMoving = true;
    _stopRequested = false;
    if (_enPin != GPIO_NUM_NC) gpio_set_level(_enPin, 0); // Enable
}

void MotorController::stop() {
    _stopRequested = true;
}

void MotorController::motorTask(void *pvParameters) {
    MotorController *self = static_cast<MotorController *>(pvParameters);
    self->run();
}

void MotorController::run() {
    while (1) {
        if (_isMoving && !_stopRequested && _currentPos != _targetPos) {
            bool dir = (_targetPos > _currentPos);
            gpio_set_level(_dirPin, dir ? 1 : 0);
            
            // Simple stepping
            gpio_set_level(_stepPin, 1);
            esp_rom_delay_us(2);
            gpio_set_level(_stepPin, 0);
            
            if (dir) _currentPos++;
            else _currentPos--;

            // Calculate delay based on speed
            if (_speed > 0) {
                uint32_t delay_us = (uint32_t)(1000000.0f / _speed);
                if (delay_us > 10) {
                    esp_rom_delay_us(delay_us - 2);
                }
            }
        } else {
            if (_isMoving) {
                _isMoving = false;
                if (_enPin != GPIO_NUM_NC) gpio_set_level(_enPin, 1); // Disable
                ESP_LOGI(TAG, "Motor stopped at %d", (int)_currentPos);
            }
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
}
#endif // !JFIX_EMULATION
