/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Physical_Control.md
 * Line: 1307
 * Language: c
 * Block ID: 78bf8e16
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// 时钟门控: 不用的外设关闭时钟，减少功耗
// ============================================================

// 外设时钟管理
typedef enum {
    PERIPH_GPIOA = 0,
    PERIPH_GPIOB,
    PERIPH_USART1,
    PERIPH_USART2,
    PERIPH_SPI1,
    PERIPH_ADC1,
    PERIPH_TIM2,
    // ...
    PERIPH_COUNT
} PeriphID;

typedef struct {
    volatile uint32_t *enr;  // 时钟使能寄存器地址
    uint32_t bit;
    uint32_t use_count;      // 引用计数
} PeriphClock;

static PeriphClock periph_clocks[] = {
    [PERIPH_GPIOA]  = {&RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN, 0},
    [PERIPH_GPIOB]  = {&RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN, 0},
    [PERIPH_USART1] = {&RCC->APB2ENR, RCC_APB2ENR_USART1EN, 0},
    // ...
};

// 使能外设时钟 (引用计数)
void periph_clock_enable(PeriphID id) {
    if (id >= PERIPH_COUNT) return;

    PeriphClock *pc = &periph_clocks[id];

    if (pc->use_count == 0) {
        *pc->enr |= pc->bit;  // 首次使用，开启时钟
    }
    pc->use_count++;
}

// 关闭外设时钟
void periph_clock_disable(PeriphID id) {
    if (id >= PERIPH_COUNT) return;

    PeriphClock *pc = &periph_clocks[id];

    if (pc->use_count > 0) {
        pc->use_count--;
        if (pc->use_count == 0) {
            *pc->enr &= ~pc->bit;  // 无人使用，关闭时钟
        }
    }
}
