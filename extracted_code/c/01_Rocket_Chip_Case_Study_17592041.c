/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\01_Rocket_Chip_Case_Study.md
 * Line: 741
 * Language: c
 * Block ID: 17592041
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// platform.h - 平台抽象层
#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>

// 内存映射区域定义
#define DDR_BASE        0x80000000UL
#define DDR_SIZE        0x40000000UL  // 1GB

#define UART_BASE       0x10000000UL
#define SPI_BASE        0x10001000UL
#define GPIO_BASE       0x10002000UL
#define CLINT_BASE      0x02000000UL
#define PLIC_BASE       0x0C000000UL

// UART寄存器
struct uart_regs {
    volatile uint32_t txdata;
    volatile uint32_t rxdata;
    volatile uint32_t txctrl;
    volatile uint32_t rxctrl;
    volatile uint32_t ie;
    volatile uint32_t ip;
    volatile uint32_t div;
};

#define UART0 ((struct uart_regs *)UART_BASE)

// 内联驱动函数
static inline void uart_init(int baudrate) {
    uint32_t divisor = F_CPU / baudrate - 1;
    UART0->div = divisor;
    UART0->txctrl = 0x1;  // 使能TX
    UART0->rxctrl = 0x1;  // 使能RX
}

static inline void uart_putc(char c) {
    while ((UART0->txdata & 0x80000000) != 0);
    UART0->txdata = c;
}

static inline char uart_getc(void) {
    uint32_t data;
    do {
        data = UART0->rxdata;
    } while ((data & 0x80000000) != 0);
    return (char)(data & 0xFF);
}

#endif
