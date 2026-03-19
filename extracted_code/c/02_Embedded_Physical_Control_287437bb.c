/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Physical_Control.md
 * Line: 360
 * Language: c
 * Block ID: 287437bb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// 寄存器数组: 批量操作多个相似外设
// ============================================================

// 方法1: 基址数组 (多个GPIO端口)
GPIO_TypeDef * const GPIO_PORTS[] = {GPIOA, GPIOB, GPIOC, GPIOD, GPIOE};
#define NUM_GPIO_PORTS 5

void gpio_init_all_input(void) {
    for (int i = 0; i < NUM_GPIO_PORTS; i++) {
        GPIO_PORTS[i]->MODER = 0;  // 全部设为输入 (00)
    }
}

// 方法2: 基址 + 偏移计算 (更紧凑)
#define GPIO_BASE_ADDR  0x40020000
#define GPIO_PORT_SIZE  0x400       // 每个GPIO端口占用1KB
#define NUM_PORTS       8

static inline GPIO_TypeDef *gpio_port(int port) {
    return (GPIO_TypeDef *)(GPIO_BASE_ADDR + port * GPIO_PORT_SIZE);
}

void gpio_set_all(uint32_t pin, int value) {
    for (int port = 0; port < NUM_PORTS; port++) {
        if (value) {
            gpio_port(port)->BSRR = (1U << pin);
        } else {
            gpio_port(port)->BSRR = (1U << (pin + 16));
        }
    }
}

// 方法3: 位带批量操作 (原子设置多个端口同一位)
void gpio_set_all_pin0_atomic(int value) {
    // 所有GPIO端口的ODR bit0的位带地址
    volatile uint32_t * const pin0_bits[] = {
        BITBAND_PTR(GPIOA_BASE + 0x14, 0),
        BITBAND_PTR(GPIOB_BASE + 0x14, 0),
        // ...
    };

    for (int i = 0; i < NUM_PORTS; i++) {
        *pin0_bits[i] = value;  // 每条都是原子操作
    }
}
