/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\12_Compiler_Optimization\README.md
 * Line: 293
 * Language: c
 * Block ID: f9857b0a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 优化前
for (int i = 0; i < n; i++) {
    int x = a + b;      // 循环不变
    arr[i] = x * i;
}

// 优化后
int x = a + b;          // 外提到循环前
for (int i = 0; i < n; i++) {
    arr[i] = x * i;
}
