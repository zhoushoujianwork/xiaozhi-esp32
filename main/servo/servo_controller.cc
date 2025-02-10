#include "servo_controller.h"
#include <esp_log.h>

#define TAG "ServoController"

ServoController &ServoController::GetInstance()
{
    static ServoController instance;
    return instance;
}

esp_err_t ServoController::Initialize(gpio_num_t pin, uint8_t channel)
{
    if (initialized_)
    {
        return ESP_OK;
    }

    // 配置 LEDC timer
    ledc_timer_config_t timer_conf = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_20_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = SERVO_FREQ,
        .clk_cfg = LEDC_AUTO_CLK};
    ESP_ERROR_CHECK(ledc_timer_config(&timer_conf));

    // 配置 LEDC channel
    channel_config_.gpio_num = pin;
    channel_config_.speed_mode = LEDC_LOW_SPEED_MODE;
    channel_config_.channel = static_cast<ledc_channel_t>(channel);
    channel_config_.timer_sel = LEDC_TIMER_0;
    channel_config_.intr_type = LEDC_INTR_DISABLE;
    channel_config_.duty = 0;
    channel_config_.hpoint = 0;

    ESP_ERROR_CHECK(ledc_channel_config(&channel_config_));

    initialized_ = true;
    ESP_LOGI(TAG, "Servo initialized on pin %d", pin);
    return ESP_OK;
}

esp_err_t ServoController::SetAngle(float angle)
{
    if (!initialized_)
    {
        return ESP_ERR_INVALID_STATE;
    }

    if (angle < 0 || angle > 180)
    {
        return ESP_ERR_INVALID_ARG;
    }

    // 将角度转换为占空比
    // 通常舵机需要0.5ms-2.5ms脉冲，对应0-180度
    // 在50Hz下，20ms周期，对应0.5ms-2.5ms是2.5%-12.5%占空比
    uint32_t duty = (angle / 180.0) * (1 << TIMER_RESOLUTION) * 0.1 + (1 << TIMER_RESOLUTION) * 0.025;

    ESP_ERROR_CHECK(ledc_set_duty(channel_config_.speed_mode,
                                  channel_config_.channel,
                                  duty));
    ESP_ERROR_CHECK(ledc_update_duty(channel_config_.speed_mode,
                                     channel_config_.channel));

    return ESP_OK;
}