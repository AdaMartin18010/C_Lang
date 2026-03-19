/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\08_Embedded_Systems.md
 * Line: 197
 * Language: c
 * Block ID: f74302c2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 方式1: 直接地址
#define GPIOA_ODR  (*(volatile uint32_t *)0x40020014)
GPIOA_ODR = 0xFF;  // 设置GPIOA输出

// 方式2: 结构体映射 (推荐)
typedef struct {
    volatile uint32_t MODER;    // 0x00
    volatile uint32_t OTYPER;   // 0x04
    volatile uint32_t OSPEEDR;  // 0x08
    volatile uint32_t PUPDR;    // 0x0C
    volatile uint32_t IDR;      // 0x10
    volatile uint32_t ODR;      // 0x14
    volatile uint32_t BSRR;     // 0x18
    volatile uint32_t LCKR;     // 0x1C
} GPIO_TypeDef;

#define GPIOA ((GPIO_TypeDef *)0x40020000)

// 使用
GPIOA->MODER |= (1 << 0);   // 设置PA0为输出
GPIOA->ODR |= (1 << 0);     // PA0输出高

// volatile关键字至关重要！
// 告诉编译器不要优化掉这些访问
