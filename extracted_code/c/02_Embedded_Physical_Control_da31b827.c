/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Physical_Control.md
 * Line: 102
 * Language: c
 * Block ID: da31b827
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// 物理视角: 寄存器是硬件的"软件接口"
// ============================================================

// 物理 reality: 0x40020000 地址对应GPIOA控制器的硬件寄存器
// 软件 abstraction: 通过C语言类型系统赋予语义

// 反模式: 直接硬编码（丧失语义）
*(volatile uint32_t *)0x40020000 = 0x01;  // 什么意思？谁知道！

// 正模式: 语义化抽象
#define GPIOA_MODER  (*(volatile uint32_t *)0x40020000)
GPIOA_MODER = GPIO_MODER_MODE0_OUTPUT;  // 清晰: 设置PA0为输出模式

// 进阶模式: 结构体映射（类型安全 + 偏移自动计算）
typedef struct {
    volatile uint32_t MODER;    // 0x00: 模式配置 (00=输入, 01=输出, 10=复用, 11=模拟)
    volatile uint32_t OTYPER;   // 0x04: 输出类型 (0=推挽, 1=开漏)
    volatile uint32_t OSPEEDR;  // 0x08: 输出速度 (00=2MHz, 01=25MHz, 10=50MHz, 11=100MHz)
    volatile uint32_t PUPDR;    // 0x0C: 上下拉   (00=浮空, 01=上拉, 10=下拉, 11=保留)
    volatile uint32_t IDR;      // 0x10: 输入数据 (只读)
    volatile uint32_t ODR;      // 0x14: 输出数据 (读写)
    volatile uint32_t BSRR;     // 0x18: 位设置/清除寄存器 (写1有效, 原子操作)
    volatile uint32_t LCKR;     // 0x1C: 锁定寄存器
    volatile uint32_t AFRL;     // 0x20: 复用功能低 [7:0]
    volatile uint32_t AFRH;     // 0x24: 复用功能高 [15:8]
} GPIO_TypeDef;

// 语义: GPIOA是一个GPIO_TypeDef类型的对象，位于0x40020000
#define GPIOA_BASE  0x40020000
#define GPIOA       ((GPIO_TypeDef *)GPIOA_BASE)

// 使用: 语法即语义
void gpio_init_output(GPIO_TypeDef *gpio, uint32_t pin) {
    // 清除模式位 (先清后设，避免影响其他位)
    gpio->MODER &= ~(3U << (pin * 2));           // 11b << pos → 清除两位
    gpio->MODER |= (1U << (pin * 2));            // 01b = 输出模式

    // 配置输出类型: 推挽 (0)
    gpio->OTYPER &= ~(1U << pin);

    // 配置速度: 50MHz (10b)
    gpio->OSPEEDR &= ~(3U << (pin * 2));
    gpio->OSPEEDR |= (2U << (pin * 2));
}
