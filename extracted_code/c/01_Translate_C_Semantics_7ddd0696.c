/*
 * Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\01_Translate_C_Semantics.md
 * Line: 1357
 * Language: c
 * Block ID: 7ddd0696
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 头文件: simple_math.h
#ifndef SIMPLE_MATH_H
#define SIMPLE_MATH_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 常量定义 */
#define PI 3.14159265358979323846
#define E 2.71828182845904523536
#define MAX_ITERATIONS 1000
#define EPSILON 1e-10

/* 错误码 */
typedef enum {
    MATH_OK = 0,
    MATH_ERROR_INVALID_INPUT = -1,
    MATH_ERROR_DIV_BY_ZERO = -2,
    MATH_ERROR_OVERFLOW = -3
} MathError;

/* 向量结构体 */
typedef struct {
    double x;
    double y;
    double z;
} Vec3;

/* 数学上下文 */
typedef struct MathContext MathContext;

/* 函数声明 */
MathError vec3_add(const Vec3* a, const Vec3* b, Vec3* result);
MathError vec3_scale(const Vec3* v, double factor, Vec3* result);
double vec3_length(const Vec3* v);

MathContext* math_context_create(void);
void math_context_destroy(MathContext* ctx);
MathError math_solve_quadratic(double a, double b, double c,
                               double* root1, double* root2);

/* 回调函数类型 */
typedef void (*MathProgressCallback)(int step, int total, void* user_data);
MathError math_iterate(MathProgressCallback callback, void* user_data);

#ifdef __cplusplus
}
#endif

#endif /* SIMPLE_MATH_H */
