/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\09_Interrupt_Hardware\01_Interrupt_Controller.md
 * Line: 1116
 * Language: c
 * Block ID: 96676f1f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * Linux内核中断栈管理
 * arch/x86/include/asm/processor.h
 */

// 每个CPU的中断栈定义
#define IRQ_STACK_ORDER     2
#define IRQ_STACK_SIZE      (PAGE_SIZE << IRQ_STACK_ORDER)  // 16KB

struct irq_stack {
    u32 stack[IRQ_STACK_SIZE / sizeof(u32)];
} __aligned(IRQ_STACK_SIZE);

// 当前CPU的中断栈指针
DECLARE_PER_CPU(struct irq_stack *, irq_stack_ptr);

/*
 * 中断栈切换代码
 * arch/x86/entry/entry_64.S
 */

/* 切换到中断栈 */
.macro SWITCH_TO_INTERRUPT_STACK
    /* 检查是否已经在中断栈上 */
    movq    %rsp, %rax
    andq    $(~(IRQ_STACK_SIZE-1)), %rax
    cmpq    %rax, PER_CPU_VAR(irq_stack_ptr)
    je      1f                      /* 已经在IRQ栈上，跳过 */

    /* 切换到IRQ栈 */
    movq    %rsp, PER_CPU_VAR(irq_stack_backup)
    movq    PER_CPU_VAR(irq_stack_ptr), %rsp

1:
.endm

/* 从中断栈恢复 */
.macro RESTORE_FROM_INTERRUPT_STACK
    /* 检查是否需要恢复 */
    cmpq    PER_CPU_VAR(irq_stack_ptr), %rsp
    jbe     1f                      /* 如果低于IRQ栈基址，不需要恢复 */

    /* 恢复原栈 */
    movq    PER_CPU_VAR(irq_stack_backup), %rsp
1:
.endm

// 双重故障栈 (Double Fault Handler栈)
// 在x86_64上，使用IST (Interrupt Stack Table) 机制
#define DOUBLEFAULT_STACK   1
#define NMI_STACK           2
#define DEBUG_STACK         3
#define MCE_STACK           4

// IST条目在TSS中的偏移
#define IST_INDEX_DF        0   // 双重故障
#define IST_INDEX_NMI       1   // NMI
#define IST_INDEX_DB        2   // 调试
#define IST_INDEX_MCE       3   // 机器检查
#define IST_INDEX_VC        4   // VMM通信
