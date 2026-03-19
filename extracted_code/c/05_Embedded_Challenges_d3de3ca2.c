/*
 * Auto-generated from: 12_Practice_Exercises\05_Embedded_Challenges.md
 * Line: 16
 * Language: c
 * Block ID: d3de3ca2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 假设STM32F4系列，地址从参考手册获取
#define GPIOA_BASE      0x40020000
#define RCC_BASE        0x40023800

#define RCC_AHB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x30))
#define GPIOA_MODER     (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_ODR       (*(volatile uint32_t *)(GPIOA_BASE + 0x14))
#define GPIOA_IDR       (*(volatile uint32_t *)(GPIOA_BASE + 0x10))

// 使能GPIOA时钟
void gpioa_clock_enable(void) {
    RCC_AHB1ENR |= (1 << 0);  // 设置位0
}

// 配置PA5为输出 (LED)
void led_init(void) {
    gpioa_clock_enable();
    GPIOA_MODER &= ~(3 << 10);   // 清除模式位
    GPIOA_MODER |= (1 << 10);    // 设置为输出模式
}

// 设置LED状态
void led_set(bool on) {
    if (on) {
        GPIOA_ODR |= (1 << 5);   // 置位
    } else {
        GPIOA_ODR &= ~(1 << 5);  // 清零
    }
}

// 简单延时
void delay_ms(uint32_t ms) {
    for (volatile uint32_t i = 0; i < ms * 16000; i++);
}

int main(void) {
    led_init();

    while (1) {
        led_set(true);
        delay_ms(500);
        led_set(false);
        delay_ms(500);
    }
}
