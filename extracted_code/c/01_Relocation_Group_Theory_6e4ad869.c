/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\08_Linking_Loading_Topology\01_Relocation_Group_Theory.md
 * Line: 420
 * Language: c
 * Block ID: 6e4ad869
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：强符号多次定义
// file1.c
int global_var = 1;  // 强符号

// file2.c
int global_var = 2;  // 强符号冲突！

// 链接错误：multiple definition

// 正确：使用extern声明
// file1.c
int global_var = 1;

// file2.c
extern int global_var;  // 引用而非定义
