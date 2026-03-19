/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\06_C_Assembly_Mapping\01_Compilation_Functor.md
 * Line: 349
 * Language: c
 * Block ID: e69b24d6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 编译器可能利用UB进行激进优化
int f(int x) {
    return (x + 1) > x;  // 编译器可能优化为 return 1;
}
// 当x = INT_MAX时，x+1溢出是UB
