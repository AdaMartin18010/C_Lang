/*
 * Auto-generated from: 08_Zig_C_Connection\04_Rust_C_Interop_Safety.md
 * Line: 913
 * Language: c
 * Block ID: 5721738a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* Rust数学库C头文件 */
#ifndef RUST_MATH_LIB_H
#define RUST_MATH_LIB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

/* ============================================
   类型定义
   ============================================ */

/* 点结构体 */
typedef struct {
    double x;
    double y;
} Point2D;

/* 错误代码 */
typedef enum {
    SUCCESS = 0,
    NULL_POINTER = 1,
    INVALID_ARGUMENT = 2,
    OUT_OF_MEMORY = 3,
    COMPUTATION_ERROR = 4
} ErrorCode;

/* ============================================
   基本数学函数
   ============================================ */

/* 计算两个整数的和 */
int rust_add(int a, int b);

/* 计算两个浮点数的乘积 */
double rust_multiply(double a, double b);

/* 计算阶乘 */
int rust_factorial(int n);

/* ============================================
   字符串处理
   ============================================ */

/* 处理字符串并返回新字符串 */
char* rust_process_string(const char* input);

/* 释放Rust分配的字符串 */
void rust_free_string(char* s);

/* ============================================
   数据结构操作
   ============================================ */

/* 计算两点之间的距离 */
double rust_point_distance(Point2D p1, Point2D p2);

/* 计算向量长度 */
double rust_vector_magnitude(const Point2D* p);

/* 批量处理点数组 */
size_t rust_process_points(
    const Point2D* points,
    size_t count,
    double* results
);

/* ============================================
   错误处理
   ============================================ */

/* 安全除法 */
ErrorCode rust_safe_divide(
    double numerator,
    double denominator,
    double* result
);

/* 获取错误消息 */
const char* rust_error_message(ErrorCode code);

/* ============================================
   内存管理
   ============================================ */

/* 分配内存 */
unsigned char* rust_alloc(size_t size);

/* 释放内存 */
void rust_free(unsigned char* ptr, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* RUST_MATH_LIB_H */
