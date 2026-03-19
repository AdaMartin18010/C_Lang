/*
 * Auto-generated from: 06_Thinking_Representation\01_Decision_Trees\07_Memory_Allocation_Decision_Tree.md
 * Line: 205
 * Language: c
 * Block ID: ba158987
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ✅ 安全的malloc使用
size_t n = 100;
size_t size = sizeof(int);
if (n > SIZE_MAX / size) {
    // 溢出处理
    return NULL;
}
int *arr = malloc(n * size);
if (arr == NULL) {
    // 分配失败处理
    return NULL;
}

// ✅ 安全的realloc使用
void *tmp = realloc(arr, new_size);
if (tmp != NULL) {
    arr = tmp;
} else {
    // arr仍然有效，可以安全使用
    // 处理错误但不丢失原数据
}

// ✅ 对齐分配
#include <stdalign.h>
double *simd_arr = aligned_alloc(32, 1024 * sizeof(double));
if (simd_arr) {
    // 使用32字节对齐的数组
    free(simd_arr);
}
