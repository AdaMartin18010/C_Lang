/*
 * Auto-generated from: 00_VERSION_TRACKING\LLVM_22_C23_Update.md
 * Line: 335
 * Language: c
 * Block ID: 43ba5098
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 库实现模式：内部辅助函数 + 外部接口

// math_utils.h
#ifndef MATH_UTILS_H
#define MATH_UTILS_H

// 内部辅助函数 (static inline)
static inline double square_internal(double x) {
    return x * x;
}

// 外部接口 (extern inline)
extern inline double hypotenuse(double a, double b) {
    return sqrt(square_internal(a) + square_internal(b));
}

#endif
