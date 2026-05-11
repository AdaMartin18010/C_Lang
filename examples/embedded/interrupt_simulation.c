/*
 * 中断向量表与处理模拟示例
 * 编译: gcc -Wall -Wextra -o interrupt_simulation interrupt_simulation.c
 */
#include <stdio.h>
#include <stdint.h>

#define NUM_IRQS 16

typedef void (*IrqHandler)(void);

static IrqHandler vector_table[NUM_IRQS];
static volatile uint32_t pending_irqs = 0;
static volatile uint32_t enabled_irqs = 0;

void irq_register(int irq_num, IrqHandler handler) {
    if (irq_num >= 0 && irq_num < NUM_IRQS) {
        vector_table[irq_num] = handler;
    }
}

void irq_enable(int irq_num) {
    if (irq_num >= 0 && irq_num < NUM_IRQS) {
        enabled_irqs |= (1U << irq_num);
    }
}

void irq_disable(int irq_num) {
    if (irq_num >= 0 && irq_num < NUM_IRQS) {
        enabled_irqs &= ~(1U << irq_num);
    }
}

void irq_trigger(int irq_num) {
    if (irq_num >= 0 && irq_num < NUM_IRQS) {
        pending_irqs |= (1U << irq_num);
    }
}

void irq_dispatch(void) {
    uint32_t pending = pending_irqs & enabled_irqs;
    while (pending) {
        int irq = __builtin_ctz(pending);
        pending_irqs &= ~(1U << irq);
        pending &= ~(1U << irq);
        if (vector_table[irq]) {
            printf("  [Dispatch] IRQ %d -> handler\n", irq);
            vector_table[irq]();
        }
    }
}

/* 具体中断处理 */
void timer_irq_handler(void) {
    printf("  Timer tick!\n");
}

void uart_rx_handler(void) {
    printf("  UART data received!\n");
}

void button_handler(void) {
    printf("  Button pressed!\n");
}

int main(void) {
    /* 初始化向量表 */
    irq_register(0, timer_irq_handler);
    irq_register(1, uart_rx_handler);
    irq_register(2, button_handler);
    
    irq_enable(0);
    irq_enable(1);
    
    printf("Triggering IRQs...\n");
    irq_trigger(0);  /* Timer - enabled */
    irq_trigger(1);  /* UART - enabled */
    irq_trigger(2);  /* Button - disabled, should not fire */
    
    irq_dispatch();
    
    printf("Enable button and trigger again...\n");
    irq_enable(2);
    irq_trigger(2);
    irq_dispatch();
    
    return 0;
}
