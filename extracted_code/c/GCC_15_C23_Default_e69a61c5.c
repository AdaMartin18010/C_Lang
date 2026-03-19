/*
 * Auto-generated from: 00_VERSION_TRACKING\GCC_15_C23_Default.md
 * Line: 1716
 * Language: c
 * Block ID: e69a61c5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 传统循环向量化困难的情况 */
#include <stddef.h>
#include <stdbool.h>

/* GCC 15 可以向量化的模式 */
int find_first_positive(const int* arr, size_t n) {
    /* 早期 break 循环 - GCC 15 可以更好地向量化 */
    for (size_t i = 0; i < n; i++) {
        if (arr[i] > 0) {
            return (int)i;  /* 早期退出 */
        }
    }
    return -1;
}

/* 另一个可优化模式 */
bool has_negative(const int* arr, size_t n) {
    /* 提前终止的搜索 */
    for (size_t i = 0; i < n; i++) {
        if (arr[i] < 0) {
            return true;  /* 早期返回 */
        }
    }
    return false;
}
