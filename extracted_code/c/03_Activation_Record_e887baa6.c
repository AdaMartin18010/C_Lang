/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\06_C_Assembly_Mapping\03_Activation_Record.md
 * Line: 300
 * Language: c
 * Block ID: e887baa6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 参数寄存器的使用
#include <stdint.h>

// 直接访问参数寄存器（GCC内联汇编）
int64_t get_first_param(void) {
    int64_t result;
    __asm__ volatile("mov %%rdi, %0" : "=r"(result));
    return result;
}

// 修改参数寄存器
void modify_params(int a, int b) {
    __asm__ volatile(
        "add $1, %%rdi\n\t"
        "add $2, %%rsi"
        :  // 无输出
        :  // 输入已隐含
        : "rdi", "rsi"  // 破坏描述
    );
    // 注意：这不会修改C变量a和b，只修改寄存器
}
