/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\README.md
 * Line: 113
 * Language: c
 * Block ID: 695ef34e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C代码
long add(long a, long b) {
    return a + b;
}

// 编译后的机器码 (add %rsi, %rdi)
// 48 01 F7
// 48: REX.W (64位操作)
// 01: ADD r/m64, r64
// F7: ModR/M = 11 110 111 = mod=3, reg=rsi(6), r/m=rdi(7)

// 反汇编验证
// objdump -d -M intel binary
