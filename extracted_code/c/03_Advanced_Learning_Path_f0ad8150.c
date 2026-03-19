/*
 * Auto-generated from: 06_Thinking_Representation\06_Learning_Paths\03_Advanced_Learning_Path.md
 * Line: 300
 * Language: c
 * Block ID: f0ad8150
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#ifndef SIMD_MATRIX_H
#define SIMD_MATRIX_H

#include <stddef.h>

// 矩阵定义
typedef struct {
    float *data;
    size_t rows;
    size_t cols;
    size_t stride;  // 行步长，支持子矩阵
} Matrix;

// 创建和销毁
Matrix* mat_create(size_t rows, size_t cols);
Matrix* mat_create_aligned(size_t rows, size_t cols, size_t alignment);
void mat_destroy(Matrix *mat);

// 基础运算（SSE/AVX实现）
void mat_add(const Matrix *a, const Matrix *b, Matrix *result);
void mat_sub(const Matrix *a, const Matrix *b, Matrix *result);
void mat_mul_scalar(const Matrix *a, float scalar, Matrix *result);

// 矩阵乘法（分块+SIMD优化）
void mat_mul(const Matrix *a, const Matrix *b, Matrix *result);
void mat_mul_blocked(const Matrix *a, const Matrix *b, Matrix *result,
                     size_t block_size);
void mat_mul_simd(const Matrix *a, const Matrix *b, Matrix *result);
void mat_mul_avx512(const Matrix *a, const Matrix *b, Matrix *result);

// 转置（缓存友好）
void mat_transpose(const Matrix *src, Matrix *dst);
void mat_transpose_blocked(const Matrix *src, Matrix *dst, size_t block_size);

// 性能对比测试
void benchmark_all_multiplications(size_t n);

#endif
