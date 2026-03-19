/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\00_Core_Semantics_Foundations\05_Undefined_Behavior_Semantics.md
 * Line: 476
 * Language: c
 * Block ID: 87577693
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

int f(int x) {
    if (x < x + 1) {
        // 编译器可假设此分支总是执行
        // 因为有符号溢出是UB
        return 1;
    }
    return 0;
}
// GCC/O2: 总是返回1
