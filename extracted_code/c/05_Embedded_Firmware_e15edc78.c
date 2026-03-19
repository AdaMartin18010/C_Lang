/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\05_Embedded_Firmware.md
 * Line: 322
 * Language: c
 * Block ID: e15edc78
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// interrupt_manager.c
#include "interrupt_manager.h"

#define MAX_IRQ_HANDLERS 96

static irq_handler_t irq_handlers[MAX_IRQ_HANDLERS];

void irq_register(uint8_t irq_num, irq_handler_t handler,
                  irq_priority_t priority) {
    // 保存处理函数
    irq_handlers[irq_num] = handler;

    // 配置优先级 (NVIC)
    uint8_t reg = irq_num / 4;
    uint8_t off = (irq_num % 4) * 8;

    // 设置优先级 (使用4位优先级)
    NVIC->IP[reg] = (NVIC->IP[reg] & ~(0xFF << off)) |
                    (priority << (off + 4));
}

void irq_enable(uint8_t irq_num) {
    NVIC->ISER[irq_num / 32] = (1 << (irq_num % 32));
}

void irq_disable(uint8_t irq_num) {
    NVIC->ICER[irq_num / 32] = (1 << (irq_num % 32));
}

// 弱定义默认处理函数
void __attribute__((weak)) TIM2_IRQHandler(void) {
    if (irq_handlers[TIM2_IRQn]) {
        irq_handlers[TIM2_IRQn]();
    }
}

void __attribute__((weak)) USART1_IRQHandler(void) {
    if (irq_handlers[USART1_IRQn]) {
        irq_handlers[USART1_IRQn]();
    }
}

// 其他中断处理函数...
