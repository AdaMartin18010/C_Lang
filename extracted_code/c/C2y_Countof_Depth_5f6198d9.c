/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 1712
 * Language: c
 * Block ID: 5f6198d9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// c23_compat.h
// 提供C23特性的向后兼容实现

#ifndef C23_COMPAT_H
#define C23_COMPAT_H

#include <stddef.h>

// _Countof 兼容层
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 202311L)
    // C23或更新：使用原生_Countof
    #define C23_COUNTOF(arr) _Countof(arr)
#else
    // C17或更早：使用sizeof技巧
    // 注意：这不会提供指针检查
    #define C23_COUNTOF(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

// 其他C23特性的兼容定义可以放在这里
// - nullptr
// - typeof
// - constexpr
// - etc.

#endif // C23_COMPAT_H
