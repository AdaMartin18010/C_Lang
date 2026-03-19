/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\05_Embedded_Firmware.md
 * Line: 295
 * Language: c
 * Block ID: 23c0801e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// interrupt_manager.h
#ifndef INTERRUPT_MANAGER_H
#define INTERRUPT_MANAGER_H

typedef void (*irq_handler_t)(void);

// 中断优先级配置
typedef enum {
    IRQ_PRIO_HIGHEST = 0,
    IRQ_PRIO_HIGH    = 2,
    IRQ_PRIO_MEDIUM  = 8,
    IRQ_PRIO_LOW     = 14,
    IRQ_PRIO_LOWEST  = 15
} irq_priority_t;

// API
void irq_register(uint8_t irq_num, irq_handler_t handler,
                  irq_priority_t priority);
void irq_enable(uint8_t irq_num);
void irq_disable(uint8_t irq_num);
void irq_set_pending(uint8_t irq_num);
void irq_clear_pending(uint8_t irq_num);

#endif
