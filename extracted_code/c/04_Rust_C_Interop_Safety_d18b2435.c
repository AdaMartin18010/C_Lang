/*
 * Auto-generated from: 08_Zig_C_Connection\04_Rust_C_Interop_Safety.md
 * Line: 1235
 * Language: c
 * Block ID: d18b2435
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* C数学库实现 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "c_math_lib.h"

/* 基本数学运算 */
double c_add(double a, double b) {
    return a + b;
}

double c_subtract(double a, double b) {
    return a - b;
}

double c_multiply(double a, double b) {
    return a * b;
}

double c_divide(double a, double b, int* error) {
    if (error) *error = 0;

    if (b == 0.0) {
        if (error) *error = 1;
        return 0.0;
    }

    return a / b;
}

/* 统计函数 */
double c_mean(const double* values, int count) {
    if (count <= 0 || values == NULL) {
        return 0.0;
    }

    double sum = 0.0;
    for (int i = 0; i < count; i++) {
        sum += values[i];
    }

    return sum / count;
}

double c_variance(const double* values, int count) {
    if (count <= 1 || values == NULL) {
        return 0.0;
    }

    double mean = c_mean(values, count);
    double sum_squared_diff = 0.0;

    for (int i = 0; i < count; i++) {
        double diff = values[i] - mean;
        sum_squared_diff += diff * diff;
    }

    return sum_squared_diff / (count - 1);
}

double c_standard_deviation(const double* values, int count) {
    return sqrt(c_variance(values, count));
}

/* 几何函数 */
double c_distance_2d(double x1, double y1, double x2, double y2) {
    double dx = x2 - x1;
    double dy = y2 - y1;
    return sqrt(dx * dx + dy * dy);
}

double c_distance_3d(double x1, double y1, double z1,
                     double x2, double y2, double z2) {
    double dx = x2 - x1;
    double dy = y2 - y1;
    double dz = z2 - z1;
    return sqrt(dx * dx + dy * dy + dz * dz);
}

/* 字符串工具 */
char* c_reverse_string(const char* str) {
    if (str == NULL) {
        return NULL;
    }

    size_t len = strlen(str);
    char* reversed = (char*)malloc(len + 1);

    if (reversed == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < len; i++) {
        reversed[i] = str[len - 1 - i];
    }
    reversed[len] = '\0';

    return reversed;
}

void c_free_string(char* str) {
    free(str);
}

/* 内存分配器 */
void* c_allocate(size_t size) {
    return malloc(size);
}

void c_deallocate(void* ptr) {
    free(ptr);
}

/* 回调函数 */
double c_apply_operation(double a, double b, OperationFunc op) {
    if (op == NULL) {
        return 0.0;
    }
    return op(a, b);
}
