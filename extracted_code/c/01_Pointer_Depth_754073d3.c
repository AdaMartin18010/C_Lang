/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\01_Pointer_Depth.md
 * Line: 1596
 * Language: c
 * Block ID: 754073d3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ UNSAFE: 指针算术溢出 */
int *pointer_overflow(int *base, size_t index) {
    /* 如果 index * sizeof(int) 溢出，可能回绕 */
    return base + index;  /* UB if overflow */
}

/* ❌ UNSAFE: 减法下溢 */
void pointer_underflow(void) {
    int arr[10];
    int *p = arr;
    int *q = p - 1;  /* 指向数组前 - UB if dereferenced */
    *q = 0;          /* UB! */
}

/* ✅ SAFE: 溢出检查 */
#include <stdint.h>

int *safe_pointer_add(int *base, size_t nmemb, size_t index) {
    if (base == NULL) return NULL;
    if (index >= nmemb) return NULL;

    /* 额外的溢出检查 */
    uintptr_t base_addr = (uintptr_t)base;
    size_t offset = index * sizeof(int);

    if (offset / sizeof(int) != index) {  /* 乘法溢出 */
        return NULL;
    }
    if (UINTPTR_MAX - base_addr < offset) {  /* 加法溢出 */
        return NULL;
    }

    return base + index;
}
