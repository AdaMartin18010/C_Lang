/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Systems.md
 * Line: 85
 * Language: c
 * Block ID: 62268958
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 内存映射寄存器访问模式

// 方法1: 直接地址（不推荐）
#define GPIOA_MODER   (*(volatile uint32_t *)0x40020000)
#define GPIOA_ODR     (*(volatile uint32_t *)0x40020014)

// 方法2: 结构体映射（推荐）
typedef struct {
    volatile uint32_t MODER;    // 0x00: 模式寄存器
    volatile uint32_t OTYPER;   // 0x04: 输出类型
    volatile uint32_t OSPEEDR;  // 0x08: 输出速度
    volatile uint32_t PUPDR;    // 0x0C: 上拉/下拉
    volatile uint32_t IDR;      // 0x10: 输入数据
    volatile uint32_t ODR;      // 0x14: 输出数据
    volatile uint32_t BSRR;     // 0x18: 位设置/清除
    volatile uint32_t LCKR;     // 0x1C: 锁定
    volatile uint32_t AFRL;     // 0x20: 复用功能低
    volatile uint32_t AFRH;     // 0x24: 复用功能高
} GPIO_TypeDef;

#define GPIOA_BASE  0x40020000
#define GPIOB_BASE  0x40020400
#define GPIOC_BASE  0x40020800

#define GPIOA   ((GPIO_TypeDef *)GPIOA_BASE)
#define GPIOB   ((GPIO_TypeDef *)GPIOB_BASE)
#define GPIOC   ((GPIO_TypeDef *)GPIOC_BASE)

// 使用
void gpio_set_pin(GPIO_TypeDef *GPIOx, uint16_t pin) {
    GPIOx->BSRR = (1U << pin);  // 设置输出
}

void gpio_clear_pin(GPIO_TypeDef *GPIOx, uint16_t pin) {
    GPIOx->BSRR = (1U << (pin + 16));  // 清除输出
}

// 位带操作（Cortex-M3/M4）
// 直接访问单个bit的内存区域
#define BITBAND_ADDR(addr, bitnum) (((addr) & 0xF0000000) + 0x2000000 + \\
                                    (((addr) & 0xFFFFF) << 5) + ((bitnum) << 2))
#define MEM_ADDR(addr) *((volatile uint32_t *)(addr))

// 使用位带设置GPIO
#define PA0_BIT  MEM_ADDR(BITBAND_ADDR(GPIOA_BASE + 0x14, 0))
PA0_BIT = 1;  // 原子设置PA0
