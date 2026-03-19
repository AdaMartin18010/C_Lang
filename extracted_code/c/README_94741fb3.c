/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Baremetal\README.md
 * Line: 441
 * Language: c
 * Block ID: 94741fb3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// main.c
#include <stdint.h>

// 寄存器定义
#define RCC_BASE        0x40021000
#define GPIOA_BASE      0x40010800

#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x18))
#define GPIOA_CRL       (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_ODR       (*(volatile uint32_t *)(GPIOA_BASE + 0x0C))

#define RCC_APB2ENR_IOPAEN  (1 << 2)

void delay(volatile uint32_t count) {
    while (count--);
}

int main(void) {
    // 使能 GPIOA 时钟
    RCC_APB2ENR |= RCC_APB2ENR_IOPAEN;

    // 配置 PA5 为推挽输出
    GPIOA_CRL &= ~(0xF << 20);  // 清除原有配置
    GPIOA_CRL |= (0x3 << 20);   // 输出模式，最大速度 50MHz

    while (1) {
        GPIOA_ODR ^= (1 << 5);  // 翻转 PA5
        delay(500000);
    }

    return 0;
}
