/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\06_C_Assembly_Mapping\01_Compilation_Functor.md
 * Line: 272
 * Language: c
 * Block ID: 5b60f835
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 优化前
if (1) {
    x = 1;
} else {
    x = 2;  // 死代码
}

// 优化后
x = 1;
