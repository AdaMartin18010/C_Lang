/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\RPi_Pico\01_Pico_Complete_Guide.md
 * Line: 975
 * Language: c
 * Block ID: 2216171d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file pwm_basics.c
 * @brief PWM 基础示例 - LED 调光
 */

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

#define LED_PIN     25

void pwm_init_pin(uint pin) {
    // 设置 GPIO 为 PWM 功能
    gpio_set_function(pin, GPIO_FUNC_PWM);

    // 获取该引脚对应的 PWM 切片
    uint slice_num = pwm_gpio_to_slice_num(pin);
    uint channel = pwm_gpio_to_channel(pin);

    // 配置 PWM
    pwm_config config = pwm_get_default_config();

    // 设置分频，使 PWM 频率约为 1kHz
    // PWM 频率 = 系统时钟 / (分频值 * (TOP + 1))
    // 125MHz / (125 * 1000) = 1kHz
    pwm_config_set_clkdiv(&config, 125.0f);
    pwm_config_set_wrap(&config, 999);  // TOP = 999

    // 初始占空比 0
    pwm_set_chan_level(slice_num, channel, 0);

    // 启用 PWM
    pwm_init(slice_num, &config, true);
}

void pwm_set_duty(uint pin, float duty_percent) {
    uint slice_num = pwm_gpio_to_slice_num(pin);
    uint channel = pwm_gpio_to_channel(pin);

    // 计算占空比值 (0-100% 映射到 0-999)
    uint16_t level = (uint16_t)((duty_percent / 100.0f) * 999.0f);
    pwm_set_chan_level(slice_num, channel, level);
}

int main(void) {
    stdio_init_all();

    // 初始化 PWM
    pwm_init_pin(LED_PIN);

    printf("PWM LED 调光演示\n");

    float duty = 0;
    int direction = 1;

    while (1) {
        // 设置占空比
        pwm_set_duty(LED_PIN, duty);

        // 渐变
        duty += direction * 0.5f;
        if (duty >= 100.0f) {
            duty = 100.0f;
            direction = -1;
        } else if (duty <= 0.0f) {
            duty = 0.0f;
            direction = 1;
        }

        sleep_ms(10);
    }

    return 0;
}
