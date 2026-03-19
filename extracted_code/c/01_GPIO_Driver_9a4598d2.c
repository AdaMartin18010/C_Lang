/*
 * Auto-generated from: 04_Industrial_Scenarios\15_Industrial_Communication\Physical_Layer\01_GPIO_Driver.md
 * Line: 84
 * Language: c
 * Block ID: 9a4598d2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* GPIO寄存器结构 - ARM Cortex-M典型实现 */
typedef struct {
    volatile uint32_t MODER;       /* 模式寄存器 */
    volatile uint32_t OTYPER;      /* 输出类型寄存器 */
    volatile uint32_t OSPEEDR;     /* 输出速度寄存器 */
    volatile uint32_t PUPDR;       /* 上拉/下拉寄存器 */
    volatile uint32_t IDR;         /* 输入数据寄存器 */
    volatile uint32_t ODR;         /* 输出数据寄存器 */
    volatile uint32_t BSRR;        /* 位设置/清除寄存器 */
    volatile uint32_t LCKR;        /* 锁定寄存器 */
    volatile uint32_t AFR[2];      /* 复用功能寄存器 */
} GPIO_TypeDef;

/* GPIOA基地址 - 示例 */
#define GPIOA_BASE      0x40020000
#define GPIOB_BASE      0x40020400
#define GPIOC_BASE      0x40020800

#define GPIOA           ((GPIO_TypeDef *)GPIOA_BASE)
#define GPIOB           ((GPIO_TypeDef *)GPIOB_BASE)
#define GPIOC           ((GPIO_TypeDef *)GPIOC_BASE)

/* 引脚位定义 */
#define GPIO_PIN_0      (1U << 0)
#define GPIO_PIN_1      (1U << 1)
/* ... */
#define GPIO_PIN_15     (1U << 15)
