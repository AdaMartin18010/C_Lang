/*
 * Auto-generated from: 06_Thinking_Representation\06_Learning_Paths\03_Advanced_Learning_Path.md
 * Line: 236
 * Language: c
 * Block ID: f746a5dc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 实现以下并行算法：

// 1. 并行快速排序
void parallel_qsort(void *base, size_t nmemb, size_t size,
                    int (*cmp)(const void *, const void *));

// 2. 并行归并排序
void parallel_mergesort(void *base, size_t nmemb, size_t size,
                        int (*cmp)(const void *, const void *));

// 3. 并行基数排序（针对整数）
void parallel_radixsort(int *arr, size_t n);

// 性能对比（在8核机器上）：
// - 单线程快速排序
// - 并行快速排序
// - std::sort (C++)
// - tbb::parallel_sort (Intel TBB)
