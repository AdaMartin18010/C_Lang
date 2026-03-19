/*
 * Auto-generated from: 06_Thinking_Representation\06_Learning_Paths\03_Advanced_Learning_Path.md
 * Line: 524
 * Language: c
 * Block ID: 384d5e29
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// main.c - STM32F4裸机程序

#include <stdint.h>

// 寄存器定义
#define RCC_BASE        0x40023800
#define GPIOA_BASE      0x40020000

#define RCC_AHB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x30))
#define GPIOA_MODER     (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_ODR       (*(volatile uint32_t *)(GPIOA_BASE + 0x14))

#define RCC_AHB1ENR_GPIOAEN     (1 << 0)
#define GPIO_MODER_MODER5_POS   (5 * 2)
#define GPIO_MODER_MODER5_MASK  (0x3 << GPIO_MODER_MODER5_POS)
#define GPIO_MODER_MODER5_OUT   (0x1 << GPIO_MODER_MODER5_POS)
#define GPIO_ODR_OD5            (1 << 5)

// 启动代码（在start.S中）
extern void _start(void);
extern void _estack(void);

// 向量表
__attribute__((section(".isr_vector")))
void (*const vector_table[])(void) = {
    _estack,    // 栈顶
    _start,     // 复位处理
};

// 延迟函数
void delay(volatile uint32_t count) {
    while (count--) {
        __asm__ volatile ("nop");
    }
}

// 主函数
int main(void) {
    // 使能GPIOA时钟
    RCC_AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    // 配置PA5为输出（LED）
    GPIOA_MODER &= ~GPIO_MODER_MODER5_MASK;
    GPIOA_MODER |= GPIO_MODER_MODER5_OUT;

    // LED闪烁
    while (1) {
        GPIOA_ODR ^= GPIO_ODR_OD5;  // 切换LED状态
        delay(500000);
    }

    return 0;
}

// 启动代码（最小实现）
void _start(void) {
    // 初始化.bss段（清零）
    extern uint32_t _sbss, _ebss;
    for (uint32_t *p = &_sbss; p < &_ebss; p++) {
        *p = 0;
    }

    // 初始化.data段（从Flash复制到RAM）
    extern uint32_t _sdata, _edata, _sidata;
    uint32_t *src = &_sidata;
    for (uint32_t *dst = &_sdata; dst < &_edata; dst++) {
        *dst = *src++;
    }

    // 调用main
    main();

    // 如果main返回，进入死循环
    while (1);
}
