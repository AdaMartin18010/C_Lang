/*
 * Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\01_System_V_ABI_Zig_C.md
 * Line: 1647
 * Language: c
 * Block ID: eff7a331
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 代码: mathlib.h - C 库头文件

#ifndef MATHLIB_H
#define MATHLIB_H

#include <stddef.h>
#include <stdint.h>

// 基本数学函数
int64_t factorial(int n);
double power(double base, int exp);

// 向量操作
typedef struct {
    double x, y, z;
} Vec3;

Vec3 vec3_add(Vec3 a, Vec3 b);
double vec3_dot(Vec3 a, Vec3 b);
Vec3 vec3_scale(Vec3 v, double s);

// 回调接口
typedef double (*UnaryOp)(double);
void map_array(double* arr, size_t n, UnaryOp op);

// 错误处理
typedef enum {
    MATH_OK = 0,
    MATH_DOMAIN_ERROR = 1,
    MATH_RANGE_ERROR = 2,
} MathError;

typedef struct {
    double value;
    MathError error;
} MathResult;

MathResult safe_sqrt(double x);

#endif // MATHLIB_H
