/*
 * Auto-generated from: 08_Zig_C_Connection\04_Rust_C_Interop_Safety.md
 * Line: 1187
 * Language: c
 * Block ID: dbbfae8a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* C数学库头文件 */
#ifndef C_MATH_LIB_H
#define C_MATH_LIB_H

#ifdef __cplusplus
extern "C" {
#endif

/* 基本数学运算 */
double c_add(double a, double b);
double c_subtract(double a, double b);
double c_multiply(double a, double b);
double c_divide(double a, double b, int* error);

/* 统计函数 */
double c_mean(const double* values, int count);
double c_variance(const double* values, int count);
double c_standard_deviation(const double* values, int count);

/* 几何函数 */
double c_distance_2d(double x1, double y1, double x2, double y2);
double c_distance_3d(double x1, double y1, double z1,
                     double x2, double y2, double z2);

/* 字符串工具 */
char* c_reverse_string(const char* str);
void c_free_string(char* str);

/* 内存分配器 */
void* c_allocate(size_t size);
void c_deallocate(void* ptr);

/* 回调函数类型 */
typedef double (*OperationFunc)(double, double);

/* 使用回调的函数 */
double c_apply_operation(double a, double b, OperationFunc op);

#ifdef __cplusplus
}
#endif

#endif /* C_MATH_LIB_H */
