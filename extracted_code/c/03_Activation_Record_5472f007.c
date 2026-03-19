/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\06_C_Assembly_Mapping\03_Activation_Record.md
 * Line: 116
 * Language: c
 * Block ID: 5472f007
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 查看栈帧结构（GCC扩展）
#include <stdint.h>
#include <stdio.h>

void inspect_stack_frame(void) {
    void **rbp;
    __asm__ volatile("mov %%rbp, %0" : "=r"(rbp));

    // rbp[0] = 保存的rbp
    // rbp[1] = 返回地址
    // rbp[2...] = 参数

    printf("当前RBP: %p\n", rbp);
    printf("旧RBP: %p\n", rbp[0]);
    printf("返回地址: %p\n", rbp[1]);
}
