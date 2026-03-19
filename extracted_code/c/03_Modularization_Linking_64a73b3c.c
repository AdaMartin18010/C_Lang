/*
 * Auto-generated from: 01_Core_Knowledge_System\03_Construction_Layer\03_Modularization_Linking.md
 * Line: 294
 * Language: c
 * Block ID: 64a73b3c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 链接错误：内联函数没有外部定义
// header.h
inline int max(int a, int b) {  // 无extern
    return a > b ? a : b;
}

// file1.c
#include "header.h"
// 使用max

// file2.c
#include "header.h"
// 使用max
// 链接错误：max没有外部定义

// ✅ C99解决方案：一个文件提供外部定义
// header.h
inline int max(int a, int b);

// utils.c
#include "header.h"
extern inline int max(int a, int b);  // 强制外部定义
