/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\RPi_Pico\01_Pico_Complete_Guide.md
 * Line: 295
 * Language: c
 * Block ID: 643a54cb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file main.c
 * @brief Pico LED 闪烁示例 - 第一个程序
 */

#include "pico/stdlib.h"
#include "hardware/gpio.h"

// LED 引脚定义 (Pico 板载 LED 连接到 GPIO25)
#define LED_PIN 25

int main(void) {
    // 初始化 GPIO
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // 初始化标准 I/O (用于 USB 串口输出)
    stdio_init_all();

    printf("Pico 启动!\n");

    while (1) {
        // LED 点亮
        gpio_put(LED_PIN, 1);
        printf("LED ON\n");
        sleep_ms(500);  // 延时 500ms

        // LED 熄灭
        gpio_put(LED_PIN, 0);
        printf("LED OFF\n");
        sleep_ms(500);  // 延时 500ms
    }

    return 0;
}
