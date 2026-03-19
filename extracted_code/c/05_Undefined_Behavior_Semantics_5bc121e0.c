/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\00_Core_Semantics_Foundations\05_Undefined_Behavior_Semantics.md
 * Line: 442
 * Language: c
 * Block ID: 5bc121e0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 程序A
int f(int x) {
    return x + 1 > x;  // 可能溢出UB
}

// 程序B
int f(int x) {
    return 1;          // 总是返回1
}

// 不等价：当x = INT_MAX时，A有UB而B返回1
