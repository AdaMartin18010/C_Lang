/*
 * Auto-generated from: 01_Core_Knowledge_System\03_Construction_Layer\03_Modularization_Linking.md
 * Line: 74
 * Language: c
 * Block ID: e8189eed
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 文件1.c
int shared = 10;  // 强符号

// 文件2.c
int shared = 20;  // ❌ 多重定义错误！

// 文件1.c
int shared = 10;  // 强符号

// 文件2.c
extern int shared;  // ✅ 正确：引用文件1的定义
