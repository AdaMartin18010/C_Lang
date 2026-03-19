/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_If_Declaration_Depth.md
 * Line: 267
 * Language: c
 * Block ID: 5cc78578
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 合法声明
if (int x = 10; x > 0) { }           // ✅ 简单声明
if (const int* p = get_ptr(); p) { } // ✅ 带限定符
if (static int s = 0; s++ < 10) { }  // ✅ 静态存储期（但需谨慎）

// 非法声明
if (extern int x = 10; x > 0) { }    // ❌ extern 不能初始化
if (int f(void); true) { }           // ❌ 函数声明（无意义）
if (struct S; true) { }              // ❌ 不完整类型声明
