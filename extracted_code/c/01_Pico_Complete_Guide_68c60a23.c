/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\RPi_Pico\01_Pico_Complete_Guide.md
 * Line: 1054
 * Language: c
 * Block ID: 68c60a23
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file pwm_motor.c
 * @brief 使用 PWM 控制直流电机 (H 桥驱动)
 */

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"

// H 桥引脚定义 (L298N 或类似驱动)
#define MOTOR_PWM_PIN       16  // ENA - 速度控制
#define MOTOR_DIR_PIN1      17  // IN1 - 方向 1
#define MOTOR_DIR_PIN2      18  // IN2 - 方向 2

// 编码器引脚 (用于测速)
#define ENCODER_A_PIN       19
#define ENCODER_B_PIN       20

// 电机控制结构体
typedef struct {
    uint pwm_pin;
    uint dir_pin1;
    uint dir_pin2;
    uint pwm_slice;
    uint pwm_channel;
    uint16_t pwm_wrap;
    volatile int32_t encoder_count;
    volatile int32_t last_encoder_count;
} motor_t;

motor_t motor;

// 编码器中断处理
void encoder_callback(uint gpio, uint32_t events) {
    bool a = gpio_get(ENCODER_A_PIN);
    bool b = gpio_get(ENCODER_B_PIN);

    // 判断方向
    if (a == b) {
        motor.encoder_count++;
    } else {
        motor.encoder_count--;
    }
}

void motor_init(motor_t* m, uint pwm_pin, uint dir1, uint dir2) {
    m->pwm_pin = pwm_pin;
    m->dir_pin1 = dir1;
    m->dir_pin2 = dir2;
    m->pwm_wrap = 1000;  // TOP 值
    m->encoder_count = 0;
    m->last_encoder_count = 0;

    // 配置方向引脚
    gpio_init(dir1);
    gpio_init(dir2);
    gpio_set_dir(dir1, GPIO_OUT);
    gpio_set_dir(dir2, GPIO_OUT);
    gpio_put(dir1, 0);
    gpio_put(dir2, 0);

    // 配置 PWM
    gpio_set_function(pwm_pin, GPIO_FUNC_PWM);
    m->pwm_slice = pwm_gpio_to_slice_num(pwm_pin);
    m->pwm_channel = pwm_gpio_to_channel(pwm_pin);

    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 125.0f);  // 1kHz
    pwm_config_set_wrap(&config, m->pwm_wrap - 1);
    pwm_init(m->pwm_slice, &config, true);

    pwm_set_chan_level(m->pwm_slice, m->pwm_channel, 0);

    // 配置编码器
    gpio_init(ENCODER_A_PIN);
    gpio_init(ENCODER_B_PIN);
    gpio_set_dir(ENCODER_A_PIN, GPIO_IN);
    gpio_set_dir(ENCODER_B_PIN, GPIO_IN);
    gpio_pull_up(ENCODER_A_PIN);
    gpio_pull_up(ENCODER_B_PIN);

    // 配置编码器中断
    gpio_set_irq_enabled_with_callback(
        ENCODER_A_PIN,
        GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
        true,
        encoder_callback
    );
}

void motor_set_speed(motor_t* m, int16_t speed) {
    // speed: -1000 到 +1000
    if (speed > 0) {
        // 正转
        gpio_put(m->dir_pin1, 1);
        gpio_put(m->dir_pin2, 0);
        uint16_t level = (speed > 1000) ? 1000 : speed;
        pwm_set_chan_level(m->pwm_slice, m->pwm_channel, level);
    } else if (speed < 0) {
        // 反转
        gpio_put(m->dir_pin1, 0);
        gpio_put(m->dir_pin2, 1);
        uint16_t level = (-speed > 1000) ? 1000 : -speed;
        pwm_set_chan_level(m->pwm_slice, m->pwm_channel, level);
    } else {
        // 停止
        gpio_put(m->dir_pin1, 0);
        gpio_put(m->dir_pin2, 0);
        pwm_set_chan_level(m->pwm_slice, m->pwm_channel, 0);
    }
}

void motor_brake(motor_t* m) {
    gpio_put(m->dir_pin1, 1);
    gpio_put(m->dir_pin2, 1);
    pwm_set_chan_level(m->pwm_slice, m->pwm_channel, m->pwm_wrap);
}

int32_t motor_get_speed(motor_t* m) {
    int32_t count = m->encoder_count;
    int32_t delta = count - m->last_encoder_count;
    m->last_encoder_count = count;
    return delta;
}

int main(void) {
    stdio_init_all();

    // 初始化电机
    motor_init(&motor, MOTOR_PWM_PIN, MOTOR_DIR_PIN1, MOTOR_DIR_PIN2);

    printf("电机控制示例启动\n");

    // 速度测试
    int16_t speeds[] = {300, 600, 1000, -300, -600, -1000, 0};
    int speed_idx = 0;
    absolute_time_t last_time = get_absolute_time();

    while (1) {
        // 每 2 秒切换一次速度
        if (absolute_time_diff_us(last_time, get_absolute_time()) > 2000000) {
            last_time = get_absolute_time();

            int16_t speed = speeds[speed_idx];
            motor_set_speed(&motor, speed);
            printf("设置速度: %d\n", speed);

            speed_idx = (speed_idx + 1) % (sizeof(speeds) / sizeof(speeds[0]));
        }

        // 每 100ms 读取编码器速度
        static absolute_time_t last_speed_time = {0};
        if (absolute_time_diff_us(last_speed_time, get_absolute_time()) > 100000) {
            last_speed_time = get_absolute_time();
            int32_t rpm = motor_get_speed(&motor) * 10;  // 转换为 RPM 估算
            printf("编码器计数: %ld, 速度: %ld\n",
                   (long)motor.encoder_count, (long)rpm);
        }

        sleep_ms(10);
    }

    return 0;
}
