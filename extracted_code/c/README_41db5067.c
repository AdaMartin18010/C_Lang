/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\06_C_Assembly_Mapping\README.md
 * Line: 244
 * Language: c
 * Block ID: 41db5067
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 基本内联汇编
__asm__ __volatile__ (
    "movl $42, %eax"    // 汇编指令
);

// 扩展内联汇编：带输入输出
uint32_t result;
uint32_t input = 100;

__asm__ __volatile__ (
    "bsr %1, %0"        // 最高位扫描
    : "=r" (result)     // 输出操作数
    : "r" (input)       // 输入操作数
    : "cc"              // 修改的寄存器/标志
);
// result = 6 (100的二进制是1100100，最高位是第6位)
