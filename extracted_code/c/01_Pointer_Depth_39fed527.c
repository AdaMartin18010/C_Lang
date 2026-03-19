/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\01_Pointer_Depth.md
 * Line: 1840
 * Language: c
 * Block ID: 39fed527
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ UNSAFE: 未检查空指针 */
void null_deref(void) {
    int *p = NULL;
    *p = 10;  /* 崩溃！段错误 */
}

/* ❌ UNSAFE: 函数参数未检查 */
void process_data(int *data, size_t n) {
    for (size_t i = 0; i < n; i++) {
        data[i] = 0;  /* 如果data为NULL，崩溃 */
    }
}

/* ❌ UNSAFE: malloc 失败未检查 */
void malloc_no_check(size_t n) {
    int *arr = malloc(n * sizeof(int));  /* 可能返回NULL */
    for (size_t i = 0; i < n; i++) {
        arr[i] = i;  /* 如果arr为NULL，崩溃 */
    }
}

/* ✅ SAFE: 防御性检查 */
void process_data_safe(int *data, size_t n) {
    if (data == NULL || n == 0) {
        return;  /* early return */
    }
    for (size_t i = 0; i < n; i++) {
        data[i] = 0;
    }
}

/* ✅ SAFE: assert 在调试模式 */
#include <assert.h>

void process_with_assert(int *data) {
    assert(data != NULL && "data should not be null");
    *data = 0;  /* 如果断言通过，安全使用 */
}

/* ✅ SAFE: malloc 检查 */
void malloc_with_check(size_t n) {
    int *arr = malloc(n * sizeof(int));
    if (arr == NULL) {
        /* 处理错误 */
        perror("malloc failed");
        return;
    }
    /* 安全使用 arr */
    free(arr);
}
