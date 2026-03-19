/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\01_Nullptr_vs_Optional_Pointer.md
 * Line: 132
 * Language: c
 * Block ID: 72a42e19
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C89/C90 - 传统 NULL 定义
#include <stdio.h>

// 平台 A: #define NULL ((void*)0)
// 平台 B: #define NULL 0
// 平台 C: #define NULL 0LL

void demo_null_issues(void) {
    int* p1 = NULL;           // OK

    // 问题 1: NULL 可以隐式转换为整数
    int x = NULL;             // 某些平台允许！x = 0

    // 问题 2: 函数重载歧义 (C++ 问题，但影响 C 头文件)
    // void foo(int);
    // void foo(int*);
    // foo(NULL);  // 歧义！

    // 问题 3: sizeof 不一致
    // sizeof(NULL) 可能是 sizeof(void*) 或 sizeof(int)
}
