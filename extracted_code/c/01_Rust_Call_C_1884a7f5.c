/*
 * Auto-generated from: 03_System_Technology_Domains\16_Rust_Interoperability\01_Rust_Call_C.md
 * Line: 88
 * Language: c
 * Block ID: 1884a7f5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* mathlib.h - C库头文件 */
#ifndef MATHLIB_H
#define MATHLIB_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 基本数学运算 */
int32_t add(int32_t a, int32_t b);
int32_t subtract(int32_t a, int32_t b);

/* 内存分配与释放 */
typedef struct {
    double x;
    double y;
} Point;

Point* point_new(double x, double y);
void point_free(Point *p);
double point_distance(const Point *a, const Point *b);

/* 回调函数类型 */
typedef int32_t (*OperationFunc)(int32_t, int32_t);
int32_t apply_operation(int32_t a, int32_t b, OperationFunc op);

/* 字符串处理 */
const char* get_version(void);
char* reverse_string(const char *input);
void free_string(char *s);

/* 数组操作 */
typedef struct {
    int32_t *data;
    size_t len;
    size_t capacity;
} IntArray;

IntArray* array_new(size_t capacity);
void array_push(IntArray *arr, int32_t value);
int32_t array_get(const IntArray *arr, size_t index);
void array_free(IntArray *arr);

#ifdef __cplusplus
}
#endif

#endif /* MATHLIB_H */
