/*
 * Auto-generated from: 01_Core_Knowledge_System\03_Construction_Layer\03_Modularization_Linking.md
 * Line: 92
 * Language: c
 * Block ID: bdb0361d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// math_utils.h
#ifndef MATH_UTILS_H
#define MATH_UTILS_H

// 接口声明
#ifdef __cplusplus
extern "C" {
#endif

// 常量
#define PI 3.14159265359
#define E  2.71828182846

// 类型
typedef struct {
    double real;
    double imag;
} Complex;

// 函数声明
Complex complex_add(Complex a, Complex b);
Complex complex_mul(Complex a, Complex b);

// 内联函数（C99）
static inline double square(double x) {
    return x * x;
}

#ifdef __cplusplus
}
#endif

#endif  // MATH_UTILS_H
