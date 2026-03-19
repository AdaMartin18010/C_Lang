/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\08_Embedded_Systems.md
 * Line: 452
 * Language: c
 * Block ID: 5ee9811c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// HardFault处理程序
void HardFault_Handler(void) {
    __asm volatile (
        "tst lr, #4\n"           // 检查使用的栈
        "ite eq\n"
        "mrseq r0, msp\n"        // 使用MSP
        "mrsne r0, psp\n"        // 使用PSP
        "ldr r1, [r0, #24]\n"    // 获取返回地址
        "b HardFault_C\n"        // 调用C处理
    );
}

void HardFault_C(uint32_t *stack_frame) {
    uint32_t r0  = stack_frame[0];
    uint32_t r1  = stack_frame[1];
    uint32_t r2  = stack_frame[2];
    uint32_t r3  = stack_frame[3];
    uint32_t r12 = stack_frame[4];
    uint32_t lr  = stack_frame[5];
    uint32_t pc  = stack_frame[6];  // 出错指令
    uint32_t psr = stack_frame[7];

    // 记录故障信息
    fault_info.r0 = r0;
    fault_info.pc = pc;
    fault_info.lr = lr;
    fault_info.CFSR = SCB->CFSR;
    fault_info.HFSR = SCB->HFSR;
    fault_info.MMFAR = SCB->MMFAR;
    fault_info.BFAR = SCB->BFAR;

    // 重启或进入安全模式
    system_reset();
}
