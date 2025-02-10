#pragma once

#include <driver/ledc.h>
#include <esp_err.h>

class ServoController
{
public:
    static ServoController &GetInstance();

    esp_err_t Initialize(gpio_num_t pin, uint8_t channel = 0);
    esp_err_t SetAngle(float angle); // angle: 0-180

private:
    ServoController() = default;
    ~ServoController() = default;

    static constexpr uint32_t SERVO_FREQ = 50; // 20ms period
    static constexpr uint32_t TIMER_RESOLUTION = 20;

    ledc_channel_config_t channel_config_{};
    bool initialized_ = false;
};