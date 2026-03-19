/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\06_C_Assembly_Mapping\03_Activation_Record.md
 * Line: 480
 * Language: c
 * Block ID: d187853f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：内联汇编破坏寄存器未声明
__asm__ volatile("mov $42, %r12");  // r12未声明为破坏
// 后续代码期望r12保持不变，但实际已改变

// 正确：声明破坏的寄存器
__asm__ volatile(
    "mov $42, %r12"
    :
    :
    : "r12"  // 声明r12被修改
);
