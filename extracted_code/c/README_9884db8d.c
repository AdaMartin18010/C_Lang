/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\06_C_Assembly_Mapping\README.md
 * Line: 392
 * Language: c
 * Block ID: 9884db8d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C代码调用汇编函数
// math_asm.s (x86-64汇编)
// .globl asm_factorial
// asm_factorial:
//     mov rax, 1
// .loop:
//     cmp rdi, 1
//     jle .done
//     mul rdi
//     dec rdi
//     jmp .loop
// .done:
//     ret

// C头文件
#ifdef __cplusplus
extern "C" {
#endif

unsigned long asm_factorial(unsigned long n);

#ifdef __cplusplus
}
#endif

// C源码中使用
#include "math_asm.h"

int main() {
    printf("5! = %lu\n", asm_factorial(5));  // 输出: 120
    return 0;
}
