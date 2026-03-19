/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering\08_Code_Review_Checklist.md
 * Line: 305
 * Language: c
 * Block ID: 452f5f8e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#ifndef SAFE_COPY_H
#define SAFE_COPY_H

#include <stddef.h>

// 安全的数组元素访问
#define SAFE_ACCESS(arr, idx, size) \
    (((idx) < (size)) ? &(arr)[idx] : NULL)

// 安全的memcpy包装
static inline int safe_memcpy(void *dst, size_t dst_size,
                               const void *src, size_t src_size) {
    if (dst == NULL || src == NULL) return -1;
    size_t copy_size = (src_size < dst_size) ? src_size : dst_size;
    memcpy(dst, src, copy_size);
    return 0;
}

// 字符串安全拷贝（保证null终止）
static inline int safe_strcpy(char *dst, size_t dst_size, const char *src) {
    if (dst == NULL || src == NULL || dst_size == 0) return -1;
    size_t i;
    for (i = 0; i < dst_size - 1 && src[i] != '\0'; i++) {
        dst[i] = src[i];
    }
    dst[i] = '\0';
    return (src[i] == '\0') ? 0 : 1;  // 0=成功, 1=截断
}

#endif
