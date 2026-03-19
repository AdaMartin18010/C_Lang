/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\ESP32\01_ESP32_Complete_Guide.md
 * Line: 699
 * Language: c
 * Block ID: 541c0a47
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * ledc_pwm_example.c
 * ESP32 LEDC (PWM) 高级控制示例
 * 功能: LED 呼吸灯、舵机控制、PWM 音乐
 */

#include "driver/ledc.h"
#include "esp_err.h"

/* LEDC 配置 */
#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO          GPIO_NUM_5      /* PWM 输出引脚 */
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT  /* 13 位分辨率 */
#define LEDC_FREQUENCY          5000            /* 5 kHz 频率 */

/* 舵机配置 */
#define SERVO_FREQ              50              /* 舵机标准 50Hz */
#define SERVO_MIN_PULSE_US      500             /* 0 度: 0.5ms */
#define SERVO_MAX_PULSE_US      2500            /* 180 度: 2.5ms */

/**
 * @brief 初始化 LEDC PWM 定时器
 */
static esp_err_t ledc_pwm_init(void)
{
    /* 配置定时器 */
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_DUTY_RES,
        .timer_num        = LEDC_TIMER,
        .freq_hz          = LEDC_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    /* 配置通道 */
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_OUTPUT_IO,
        .duty           = 0,                    /* 初始占空比 0 */
        .hpoint         = 0,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

    /* 启用渐变功能 */
    ESP_ERROR_CHECK(ledc_fade_func_install(0));

    return ESP_OK;
}

/**
 * @brief 设置 PWM 占空比
 * @param duty 占空比值 (0 - 8191, 对应 13 位分辨率)
 */
static void ledc_set_duty_cycle(uint32_t duty)
{
    /* 设置占空比 */
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty));
    /* 更新占空比 */
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
}

/**
 * @brief 设置舵机角度
 * @param angle 角度 (0 - 180)
 */
static void servo_set_angle(uint8_t angle)
{
    /* 限制角度范围 */
    if (angle > 180) angle = 180;

    /* 计算脉冲宽度 */
    uint32_t pulse_us = SERVO_MIN_PULSE_US +
                        (angle * (SERVO_MAX_PULSE_US - SERVO_MIN_PULSE_US) / 180);

    /* 计算占空比 (对于 50Hz, 周期为 20ms = 20000us)
     * 占空比 = pulse_us / 20000 * 8191 (13位)
     */
    uint32_t duty = (pulse_us * 8191) / 20000;

    ledc_set_duty_cycle(duty);

    ESP_LOGI(TAG, "舵机角度: %d°, 脉冲: %d us, 占空比: %d",
             angle, (int)pulse_us, (int)duty);
}

/**
 * @brief 呼吸灯效果 (使用硬件渐变)
 */
static void breathing_led_task(void *pvParameters)
{
    const uint32_t fade_time_ms = 2000;  /* 渐变时间 2 秒 */

    while (1) {
        /* 渐亮 */
        ESP_ERROR_CHECK(ledc_set_fade_with_time(
            LEDC_MODE, LEDC_CHANNEL, 8191, fade_time_ms));
        ESP_ERROR_CHECK(ledc_fade_start(LEDC_MODE, LEDC_CHANNEL, LEDC_FADE_NO_WAIT));
        vTaskDelay(pdMS_TO_TICKS(fade_time_ms));

        /* 渐暗 */
        ESP_ERROR_CHECK(ledc_set_fade_with_time(
            LEDC_MODE, LEDC_CHANNEL, 0, fade_time_ms));
        ESP_ERROR_CHECK(ledc_fade_start(LEDC_MODE, LEDC_CHANNEL, LEDC_FADE_NO_WAIT));
        vTaskDelay(pdMS_TO_TICKS(fade_time_ms));
    }
}

/**
 * @brief 舵机扫描任务
 */
static void servo_sweep_task(void *pvParameters)
{
    uint8_t angle = 0;
    int8_t direction = 1;

    while (1) {
        servo_set_angle(angle);

        angle += direction * 5;  /* 每次增加 5 度 */

        if (angle >= 180) {
            angle = 180;
            direction = -1;
        } else if (angle == 0) {
            direction = 1;
        }

        vTaskDelay(pdMS_TO_TICKS(50));  /* 每 50ms 移动一次 */
    }
}
