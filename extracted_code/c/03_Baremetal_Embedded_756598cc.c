/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\03_Baremetal_Embedded.md
 * Line: 601
 * Language: c
 * Block ID: 756598cc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// vector_table.c - 直接向量模式中断表

#include "hal.h"

#define IRQ_COUNT 64

// 向量表入口类型
typedef void (*vector_fn_t)(void);

// 默认处理函数
void default_handler(void) {
    while (1) {
        wfi();
    }
}

// 声明外部中断处理函数
extern void reset_handler(void);
extern void nmi_handler(void);
extern void timer_handler(void);
extern void uart0_handler(void);
extern void uart1_handler(void);
extern void spi_handler(void);
extern void gpio_handler(void);
extern void dma_handler(void);

// 向量表定义
__attribute__((section(".vector_table"), aligned(256)))
const vector_fn_t vector_table[IRQ_COUNT] = {
    [0]  = reset_handler,           /* Reset */
    [1]  = nmi_handler,             /* NMI */
    [2]  = default_handler,         /* HardFault */
    [3]  = default_handler,         /* Reserved */
    [7]  = timer_handler,           /* Timer */
    [16] = uart0_handler,           /* UART0 */
    [17] = uart1_handler,           /* UART1 */
    [18] = spi_handler,             /* SPI */
    [19] = gpio_handler,            /* GPIO */
    [20] = dma_handler,             /* DMA */
    /* ... 更多中断 */
    [IRQ_COUNT - 1] = default_handler
};

// 初始化向量表
void vector_table_init(void) {
    // 设置向量表基地址
    uintptr_t vt_addr = (uintptr_t)vector_table;
    write_csr(mtvec, vt_addr | 1);  // 最低位1表示向量模式
}

// 获取中断号并跳转到处理函数
void direct_vector_handler(void) {
    uint32_t mcause;
    __asm__ volatile("csrr %0, mcause" : "=r"(mcause));

    uint32_t irq_num = mcause & 0x3FF;
    if (irq_num < IRQ_COUNT && vector_table[irq_num] != default_handler) {
        vector_table[irq_num]();
    } else {
        default_handler();
    }
}
