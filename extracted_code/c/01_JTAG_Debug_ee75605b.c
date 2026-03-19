/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\19_Hardware_Debugging\01_JTAG_Debug.md
 * Line: 2123
 * Language: c
 * Block ID: ee75605b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// hardfault_handler.c
// HardFault 处理与信息收集

typedef struct {
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t lr;
    uint32_t pc;
    uint32_t psr;
} HardFault_StackFrame_t;

typedef struct {
    uint32_t cfsr;
    uint32_t hfsr;
    uint32_t mmfar;
    uint32_t bfar;
    uint32_t afsr;
    HardFault_StackFrame_t stack_frame;
} HardFault_Info_t;

volatile HardFault_Info_t g_hardfault_info;

__attribute__((naked)) void HardFault_Handler(void) {
    __asm volatile (
        "tst lr, #4             \n"  // 检查 EXC_RETURN 位2
        "ite eq                 \n"
        "mrseq r0, msp          \n"  // 使用 MSP
        "mrsne r0, psp          \n"  // 使用 PSP
        "b HardFault_Handler_C  \n"  // 跳转到 C 处理函数
    );
}

void HardFault_Handler_C(uint32_t *stack_frame) {
    // 保存栈帧
    g_hardfault_info.stack_frame.r0  = stack_frame[0];
    g_hardfault_info.stack_frame.r1  = stack_frame[1];
    g_hardfault_info.stack_frame.r2  = stack_frame[2];
    g_hardfault_info.stack_frame.r3  = stack_frame[3];
    g_hardfault_info.stack_frame.r12 = stack_frame[4];
    g_hardfault_info.stack_frame.lr  = stack_frame[5];
    g_hardfault_info.stack_frame.pc  = stack_frame[6];
    g_hardfault_info.stack_frame.psr = stack_frame[7];

    // 保存故障状态寄存器
    g_hardfault_info.cfsr  = *(volatile uint32_t *)0xE000ED28;
    g_hardfault_info.hfsr  = *(volatile uint32_t *)0xE000ED2C;
    g_hardfault_info.mmfar = *(volatile uint32_t *)0xE000ED34;
    g_hardfault_info.bfar  = *(volatile uint32_t *)0xE000ED38;
    g_hardfault_info.afsr  = *(volatile uint32_t *)0xE000ED3C;

    // 这里可以添加断点或 LED 指示
    __asm volatile ("bkpt #0");

    // 无限循环等待调试
    while (1) {
        // 保持在此处便于调试器连接
    }
}
