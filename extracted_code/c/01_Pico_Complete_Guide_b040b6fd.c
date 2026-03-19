/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\RPi_Pico\01_Pico_Complete_Guide.md
 * Line: 488
 * Language: c
 * Block ID: b040b6fd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file gpio_advanced.c
 * @brief 高级 GPIO 操作示例
 */

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/sync.h"

// 多位 GPIO 同时操作
#define LED_MASK    ((1UL << 25) | (1UL << 24) | (1UL << 23))

void gpio_advanced_demo(void) {
    // 同时设置多个 GPIO 为输出
    for (int pin = 23; pin <= 25; pin++) {
        gpio_init(pin);
        gpio_set_dir(pin, GPIO_OUT);
    }

    // 使用掩码同时操作多个 GPIO
    gpio_set_mask(LED_MASK);    // 同时设置 bit 23,24,25 为高
    sleep_ms(500);
    gpio_clr_mask(LED_MASK);    // 同时清除 bit 23,24,25
    sleep_ms(500);
    gpio_xor_mask(LED_MASK);    // 同时翻转 bit 23,24,25

    // 原子操作示例
    uint32_t save = save_and_disable_interrupts();
    // 临界区代码 - 不会被中断打断
    gpio_put(25, 1);
    gpio_put(24, 1);
    restore_interrupts(save);

    // 驱动强度设置
    gpio_set_drive_strength(25, GPIO_DRIVE_STRENGTH_12MA);

    // 斜率控制
    gpio_set_slew_rate(25, GPIO_SLEW_RATE_FAST);
}
