/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\12_Compiler_Optimization\README.md
 * Line: 309
 * Language: c
 * Block ID: 54ffa885
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 优化前
for (int i = 0; i < 100; i++) {
    sum += arr[i];
}

// 优化后 (展开4倍)
for (int i = 0; i < 100; i += 4) {
    sum += arr[i];
    sum += arr[i+1];
    sum += arr[i+2];
    sum += arr[i+3];
}
