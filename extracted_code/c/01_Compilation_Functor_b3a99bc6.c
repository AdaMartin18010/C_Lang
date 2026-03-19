/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\06_C_Assembly_Mapping\01_Compilation_Functor.md
 * Line: 292
 * Language: c
 * Block ID: b3a99bc6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 优化前
int a = *p;
int b = *p;
int c = a + b;

// 优化后 (假设无别名)
int a = *p;
int c = a + a;  // 消除第二次加载
