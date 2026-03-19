/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Baremetal\README.md
 * Line: 549
 * Language: c
 * Block ID: bb0ae7bf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// HardFault 处理程序
void HardFault_Handler(void) {
    __asm volatile (
        "tst lr, #4\n"
        "ite eq\n"
        "mrseq r0, msp\n"
        "mrsne r0, psp\n"
        "ldr r1, [r0, #24]\n"
        "b HardFault_Handler_C\n"
    );
}

void HardFault_Handler_C(uint32_t *stackFrame) {
    uint32_t stacked_pc = stackFrame[6];
    printf("HardFault at PC: 0x%08X\n", (unsigned int)stacked_pc);
    while (1);
}
