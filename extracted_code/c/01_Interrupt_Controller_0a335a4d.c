/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\09_Interrupt_Hardware\01_Interrupt_Controller.md
 * Line: 716
 * Language: c
 * Block ID: 0a335a4d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * Linux内核 IDT 向量分配
 * arch/x86/include/asm/irq_vectors.h
 */

// CPU异常向量 (0-31)
#define X86_TRAP_DE         0   // 除零错误
#define X86_TRAP_DB         1   // 调试
#define X86_TRAP_NMI        2   // 非屏蔽中断
#define X86_TRAP_BP         3   // 断点
#define X86_TRAP_OF         4   // 溢出
#define X86_TRAP_BR         5   // 边界检查
#define X86_TRAP_UD         6   // 非法指令
#define X86_TRAP_NM         7   // 设备不可用
#define X86_TRAP_DF         8   // 双重错误
#define X86_TRAP_OLD_MF     9   // 协处理器段溢出
#define X86_TRAP_TS         10  // 无效TSS
#define X86_TRAP_NP         11  // 段不存在
#define X86_TRAP_SS         12  // 栈段错误
#define X86_TRAP_GP         13  // 通用保护错误
#define X86_TRAP_PF         14  // 页错误
#define X86_TRAP_SPURIOUS   15  // 预留/伪中断
#define X86_TRAP_MF         16  // x87浮点错误
#define X86_TRAP_AC         17  // 对齐检查
#define X86_TRAP_MC         18  // 机器检查
#define X86_TRAP_XF         19  // SIMD浮点错误
#define X86_TRAP_VE         20  // 虚拟化异常

// ISA中断向量 (传统PIC)
#define IRQ0_VECTOR         0x20
#define IRQ1_VECTOR         0x21
#define IRQ2_VECTOR         0x22
#define IRQ3_VECTOR         0x23
#define IRQ4_VECTOR         0x24
#define IRQ5_VECTOR         0x25
#define IRQ6_VECTOR         0x26
#define IRQ7_VECTOR         0x27
#define IRQ8_VECTOR         0x28
#define IRQ9_VECTOR         0x29
#define IRQ10_VECTOR        0x2A
#define IRQ11_VECTOR        0x2B
#define IRQ12_VECTOR        0x2C
#define IRQ13_VECTOR        0x2D
#define IRQ14_VECTOR        0x2E
#define IRQ15_VECTOR        0x2F

// APIC特殊向量
#define LOCAL_TIMER_VECTOR      0xEF
#define ERROR_APIC_VECTOR       0xFE
#define SPURIOUS_APIC_VECTOR    0xFF

// IPI向量
#define RESCHEDULE_VECTOR       0xF1
#define CALL_FUNCTION_VECTOR    0xF2
#define CALL_FUNCTION_SINGLE_VECTOR 0xF3
#define REBOOT_VECTOR           0xF4
