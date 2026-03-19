/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\06_C_Assembly_Mapping\03_Activation_Record.md
 * Line: 353
 * Language: c
 * Block ID: 8d1ea633
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void fixed_locals(void) {
    int a = 1;           // RBP-4
    double b = 2.0;      // RBP-16 (8字节对齐)
    char c = 'x';        // RBP-17
    // 填充到16字节对齐

    // 实际汇编：
    // mov DWORD PTR [rbp-4], 1
    // movsd xmm0, QWORD PTR .LC0[rip]
    // movsd QWORD PTR [rbp-16], xmm0
    // mov BYTE PTR [rbp-17], 120
}
