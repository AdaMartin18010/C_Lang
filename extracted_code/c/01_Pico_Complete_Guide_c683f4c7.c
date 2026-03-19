/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\RPi_Pico\01_Pico_Complete_Guide.md
 * Line: 1223
 * Language: c
 * Block ID: c683f4c7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file pwm_servo.c
 * @brief 舵机控制示例 (SG90, MG995 等)
 */

#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define SERVO_PIN   15

// 舵机参数 (SG90)
// 周期: 20ms (50Hz)
// 0度:   0.5ms 脉冲 (2.5% 占空比)
// 90度:  1.5ms 脉冲 (7.5% 占空比)
// 180度: 2.5ms 脉冲 (12.5% 占空比)

void servo_init(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);

    uint slice_num = pwm_gpio_to_slice_num(pin);
    uint channel = pwm_gpio_to_channel(pin);

    // 配置 50Hz (20ms 周期)
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 125.0f);  // 1MHz
    pwm_config_set_wrap(&config, 19999);     // 20ms 周期

    pwm_init(slice_num, &config, true);
    pwm_set_chan_level(slice_num, channel, 1500);  // 初始位置 90度
}

void servo_set_angle(uint pin, float angle) {
    // 限制角度范围
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;

    uint slice_num = pwm_gpio_to_slice_num(pin);
    uint channel = pwm_gpio_to_channel(pin);

    // 计算脉冲宽度 (微秒)
    // 0度 = 500us, 180度 = 2500us
    uint16_t pulse_us = (uint16_t)(500 + (angle / 180.0f) * 2000);

    pwm_set_chan_level(slice_num, channel, pulse_us);
}

void servo_set_microseconds(uint pin, uint16_t us) {
    if (us < 500) us = 500;
    if (us > 2500) us = 2500;

    uint slice_num = pwm_gpio_to_slice_num(pin);
    uint channel = pwm_gpio_to_channel(pin);

    pwm_set_chan_level(slice_num, channel, us);
}

int main(void) {
    stdio_init_all();

    servo_init(SERVO_PIN);

    printf("舵机控制示例启动\n");

    while (1) {
        // 从 0 度到 180 度扫描
        for (float angle = 0; angle <= 180; angle += 2) {
            servo_set_angle(SERVO_PIN, angle);
            sleep_ms(20);
        }

        sleep_ms(500);

        // 从 180 度到 0 度扫描
        for (float angle = 180; angle >= 0; angle -= 2) {
            servo_set_angle(SERVO_PIN, angle);
            sleep_ms(20);
        }

        sleep_ms(500);
    }

    return 0;
}
