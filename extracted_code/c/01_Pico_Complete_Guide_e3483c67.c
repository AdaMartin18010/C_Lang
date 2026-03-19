/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\RPi_Pico\01_Pico_Complete_Guide.md
 * Line: 417
 * Language: c
 * Block ID: e3483c67
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file gpio_interrupt.c
 * @brief GPIO 中断示例 - 按钮中断触发
 */

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"

#define BUTTON_PIN  14
#define LED_PIN     25

// 全局计数器
volatile uint32_t button_press_count = 0;
volatile bool button_event = false;

// GPIO 中断处理函数
void gpio_callback(uint gpio, uint32_t events) {
    if (gpio == BUTTON_PIN) {
        button_press_count++;
        button_event = true;
    }
}

int main(void) {
    stdio_init_all();

    // LED 配置
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // 按钮配置
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    // 配置 GPIO 中断
    // 触发条件：下降沿（按钮按下，从上拉到接地）
    gpio_set_irq_enabled_with_callback(
        BUTTON_PIN,
        GPIO_IRQ_EDGE_FALL,
        true,
        &gpio_callback
    );

    printf("GPIO 中断示例启动\n");
    printf("按按钮触发中断...\n");

    while (1) {
        if (button_event) {
            button_event = false;

            // 切换 LED 状态
            static bool led_state = false;
            led_state = !led_state;
            gpio_put(LED_PIN, led_state);

            printf("中断触发 #%lu, LED=%d\n", button_press_count, led_state);
        }

        // 低功耗等待
        __wfi();  // Wait For Interrupt
    }

    return 0;
}
