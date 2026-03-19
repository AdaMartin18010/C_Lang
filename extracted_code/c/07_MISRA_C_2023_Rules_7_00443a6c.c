/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\07_MISRA_C_2023_Rules_7.md
 * Line: 382
 * Language: c
 * Block ID: 00443a6c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#ifndef SAFE_BUFFER_H
#define SAFE_BUFFER_H

#include <string.h>
#include <stdint.h>

/* 安全字符串复制 */
static inline void safe_strcpy(char *dest, size_t dest_size, const char *src)
{
    if (dest == NULL || src == NULL || dest_size == 0) {
        return;
    }
    strncpy(dest, src, dest_size - 1);
    dest[dest_size - 1] = '\0';
}

/* 安全数组索引访问 */
#define SAFE_ARRAY_ACCESS(arr, idx, size, default_val) \
    (((idx) < (size)) ? (arr)[(idx)] : (default_val))

/* 安全的memcpy */
static inline void safe_memcpy(void *dest, size_t dest_size,
                                const void *src, size_t src_size)
{
    if (dest == NULL || src == NULL) {
        return;
    }
    size_t copy_size = (dest_size < src_size) ? dest_size : src_size;
    memcpy(dest, src, copy_size);
}

#endif
