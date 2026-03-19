/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\03_Baremetal_Embedded.md
 * Line: 668
 * Language: c
 * Block ID: 72313db3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// plic.c - 平台级中断控制器驱动

#include "hal.h"

#define PLIC_BASE       0x0C000000
#define PLIC_PRIORITY   (PLIC_BASE + 0x000000)
#define PLIC_PENDING    (PLIC_BASE + 0x001000)
#define PLIC_ENABLE     (PLIC_BASE + 0x002000)
#define PLIC_THRESHOLD  (PLIC_BASE + 0x200000)
#define PLIC_CLAIM      (PLIC_BASE + 0x200004)

#define MAX_EXTERNAL_IRQ 127

static volatile uint32_t *plic_priority = (uint32_t *)PLIC_PRIORITY;
static volatile uint32_t *plic_pending = (uint32_t *)PLIC_PENDING;
static volatile uint32_t *plic_enable = (uint32_t *)PLIC_ENABLE;
static volatile uint32_t *plic_threshold = (uint32_t *)PLIC_THRESHOLD;
static volatile uint32_t *plic_claim = (uint32_t *)PLIC_CLAIM;

// 中断优先级 (0-7, 0=禁用)
void plic_set_priority(int irq, int priority) {
    if (irq > 0 && irq <= MAX_EXTERNAL_IRQ) {
        plic_priority[irq] = priority & 0x7;
    }
}

// 使能特定中断
void plic_enable_irq(int irq) {
    if (irq > 0 && irq <= MAX_EXTERNAL_IRQ) {
        int word = irq / 32;
        int bit = irq % 32;
        plic_enable[word] |= (1 << bit);
    }
}

// 禁用特定中断
void plic_disable_irq(int irq) {
    if (irq > 0 && irq <= MAX_EXTERNAL_IRQ) {
        int word = irq / 32;
        int bit = irq % 32;
        plic_enable[word] &= ~(1 << bit);
    }
}

// 设置中断阈值
void plic_set_threshold(int threshold) {
    *plic_threshold = threshold & 0x7;
}

// 获取最高优先级待处理中断
uint32_t plic_claim(void) {
    return *plic_claim;
}

// 完成中断处理
void plic_complete(uint32_t irq) {
    *plic_claim = irq;
}

// 初始化PLIC
void plic_init(void) {
    // 禁用所有中断
    for (int i = 0; i <= MAX_EXTERNAL_IRQ / 32; i++) {
        plic_enable[i] = 0;
    }

    // 设置所有中断优先级为0 (禁用)
    for (int i = 1; i <= MAX_EXTERNAL_IRQ; i++) {
        plic_priority[i] = 0;
    }

    // 设置阈值为0 (接收所有优先级的中断)
    plic_set_threshold(0);
}
