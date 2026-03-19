/*
 * Auto-generated from: 12_Practice_Exercises\08_Concurrency_Parallelism_Labs.md
 * Line: 311
 * Language: c
 * Block ID: 4ae64f58
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 并行矩阵乘法
void matmul_parallel(const double *a, const double *b,
                     double *c, size_t n);

// 并行归约（求和、求最大最小值）
double parallel_sum(const double *arr, size_t n);
double parallel_dot_product(const double *a, const double *b, size_t n);

// 并行for循环
void parallel_transform(double *arr, size_t n,
                        double (*func)(double));

// 任务并行
void parallel_tree_traversal(TreeNode *root);

// SIMD + OpenMP
void simd_parallel_add(float *a, const float *b, size_t n);
