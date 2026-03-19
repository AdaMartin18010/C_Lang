/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\02_C17_C23_Features.md
 * Line: 69
 * Language: c
 * Block ID: 5122dea8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ C11及以前的问题
int *p1 = 0;        // 0是整数，不表达意图
int *p2 = NULL;     // 可能是 ((void*)0) 或 0
void *vp = NULL;    // 与上面可能不同

// ✅ C23: 类型安全的nullptr
int *p = nullptr;   // 明确的空指针常量
void *vp2 = nullptr;

// 比较
if (p == nullptr) { }  // 清晰表达意图

// 不能用于整数
// int x = nullptr;  // 错误！
