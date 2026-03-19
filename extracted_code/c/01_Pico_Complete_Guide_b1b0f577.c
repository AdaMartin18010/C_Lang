/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\RPi_Pico\01_Pico_Complete_Guide.md
 * Line: 370
 * Language: c
 * Block ID: b1b0f577
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file gpio_basic.c
 * @brief GPIO 基础输入输出示例
 */

#include "pico/stdlib.h"
#include "hardware/gpio.h"

// 引脚定义
#define LED_PIN     25
#define BUTTON_PIN  14

int main(void) {
    stdio_init_all();

    // LED 配置为输出
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // 按钮配置为输入，启用内部上拉电阻
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);  // 启用上拉，按钮按下时读低电平

    printf("GPIO 基础示例启动\n");

    while (1) {
        // 读取按钮状态
        bool button_pressed = !gpio_get(BUTTON_PIN);  // 取反因为上拉

        if (button_pressed) {
            gpio_put(LED_PIN, 1);  // 点亮 LED
            printf("按钮按下\n");
        } else {
            gpio_put(LED_PIN, 0);  // 熄灭 LED
        }

        sleep_ms(50);  // 简单消抖
    }

    return 0;
}
