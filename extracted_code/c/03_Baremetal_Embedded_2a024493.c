/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\03_Baremetal_Embedded.md
 * Line: 318
 * Language: c
 * Block ID: 2a024493
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// trap.c - 异常和陷阱处理

#include "hal.h"
#include <stdio.h>

typedef struct {
    uint64_t x[32];
    uint64_t mcause;
    uint64_t mepc;
    uint64_t mtval;
} trap_context_t;

// 异常原因字符串
static const char* get_exception_cause(uint64_t mcause) {
    if (mcause & (1ULL << 63)) {
        // 中断
        switch (mcause & 0xFF) {
            case 0: return "User software interrupt";
            case 1: return "Supervisor software interrupt";
            case 3: return "Machine software interrupt";
            case 4: return "User timer interrupt";
            case 5: return "Supervisor timer interrupt";
            case 7: return "Machine timer interrupt";
            case 8: return "User external interrupt";
            case 9: return "Supervisor external interrupt";
            case 11: return "Machine external interrupt";
            default: return "Unknown interrupt";
        }
    } else {
        // 异常
        switch (mcause) {
            case 0: return "Instruction address misaligned";
            case 1: return "Instruction access fault";
            case 2: return "Illegal instruction";
            case 3: return "Breakpoint";
            case 4: return "Load address misaligned";
            case 5: return "Load access fault";
            case 6: return "Store/AMO address misaligned";
            case 7: return "Store/AMO access fault";
            case 8: return "Environment call from U-mode";
            case 9: return "Environment call from S-mode";
            case 11: return "Environment call from M-mode";
            case 12: return "Instruction page fault";
            case 13: return "Load page fault";
            case 15: return "Store/AMO page fault";
            default: return "Unknown exception";
        }
    }
}

// 中断处理函数表
#define MAX_IRQ 64
typedef void (*irq_handler_t)(void);
static irq_handler_t irq_handlers[MAX_IRQ];

void register_irq_handler(int irq, irq_handler_t handler) {
    if (irq >= 0 && irq < MAX_IRQ) {
        irq_handlers[irq] = handler;
    }
}

// 主陷阱处理函数
void trap_handler(trap_context_t *ctx) {
    uint64_t mcause = ctx->mcause;

    if (mcause & (1ULL << 63)) {
        // 中断处理
        uint64_t irq_num = mcause & 0xFF;

        if (irq_num == 7) {
            // 机器定时器中断
            timer_irq_handler();
        } else if (irq_num == 11) {
            // 外部中断 - 从PLIC获取中断号
            uint32_t irq = plic_claim();
            if (irq < MAX_IRQ && irq_handlers[irq]) {
                irq_handlers[irq]();
            }
            plic_complete(irq);
        } else {
            printf("Unhandled interrupt: %s\n", get_exception_cause(mcause));
        }

        // 中断返回前增加mepc (如果不是断点)
        if (irq_num != 3) {
            ctx->mepc += 4;
        }
    } else {
        // 异常处理
        printf("\n=== TRAP ===\n");
        printf("Cause: %s (code=%lu)\n", get_exception_cause(mcause), mcause);
        printf("EPC:   0x%016lx\n", ctx->mepc);
        printf("TVAL:  0x%016lx\n", ctx->mtval);
        printf("\nRegister dump:\n");
        for (int i = 0; i < 32; i++) {
            printf("x%2d = 0x%016lx\n", i, ctx->x[i]);
        }

        // 致命异常 - 停机
        system_control(SYS_RESET);
    }
}
