/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\05_MISRA_C_2023_Rules_5.md
 * Line: 586
 * Language: c
 * Block ID: 7a2dda6d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#ifndef SAFE_TYPES_H
#define SAFE_TYPES_H

#include <stdint.h>
#include <limits.h>
#include <stdbool.h>

/* 安全整数转换 */
#define SAFE_INT_CAST(dest, src, max, min) \
    do { \
        if ((src) > (max) || (src) < (min)) { \
            /* 错误处理 */ \
        } else { \
            (dest) = (typeof(dest))(src); \
        } \
    } while(0)

/* 安全指针转换 */
#define SAFE_PTR_CAST(dest, src, dest_type) \
    do { \
        _Static_assert(sizeof(*(src)) == sizeof(*(dest)), \
                      "Type size mismatch"); \
        (dest) = (dest_type)(src); \
    } while(0)

/* 示例使用 */
void example(void)
{
    int large = 1000;
    int8_t small;

    SAFE_INT_CAST(small, large, INT8_MAX, INT8_MIN);
}

#endif
