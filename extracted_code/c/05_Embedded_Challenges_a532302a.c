/*
 * Auto-generated from: 12_Practice_Exercises\05_Embedded_Challenges.md
 * Line: 433
 * Language: c
 * Block ID: a532302a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 保存当前上下文
// 恢复下一个任务的上下文
__attribute__((naked)) void PendSV_Handler(void) {
    __asm volatile (
        "MRS     R0, PSP\n"           // 获取进程栈指针
        "STMDB   R0!, {R4-R11}\n"    // 保存R4-R11
        "LDR     R3, =scheduler\n"
        "LDR     R2, [R3, #4]\n"     // current_task
        "STR     R0, [R3, R2, LSL #2]\n"  // 保存SP

        // 选择下一个任务...

        "LDMIA   R0!, {R4-R11}\n"    // 恢复R4-R11
        "MSR     PSP, R0\n"           // 恢复PSP
        "BX      LR\n"
    );
}
