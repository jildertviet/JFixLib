#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#ifndef JFIX_EMULATION

#include "esp_err.h"
#include "hal/gpio_types.h"
#include <stdint.h>

class MotorController {
public:
    MotorController();
    esp_err_t init(gpio_num_t step_pin, gpio_num_t dir_pin, gpio_num_t en_pin);
    
    void move(int32_t steps, float speed);
    void moveTo(int32_t position, float speed);
    void stop();
    
    bool isMoving() const { return _isMoving; }
    int32_t getPosition() const { return _currentPos; }

private:
    static void motorTask(void *pvParameters);
    void run();

    gpio_num_t _stepPin;
    gpio_num_t _dirPin;
    gpio_num_t _enPin;

    int32_t _targetPos = 0;
    int32_t _currentPos = 0;
    float _speed = 1000.0f; // steps per second
    bool _isMoving = false;
    bool _stopRequested = false;
};

extern MotorController motorController;

#endif // !JFIX_EMULATION

#endif // MOTOR_CONTROLLER_H
