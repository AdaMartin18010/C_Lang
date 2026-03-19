/*
 * Auto-generated from: 04_Industrial_Scenarios\02_Automotive_ECU\03_Functional_Safety.md
 * Line: 117
 * Language: c
 * Block ID: 86e5cb39
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 防御性编程示例 */
#include <stdint.h>
#include <stdbool.h>

/* 安全除法函数 - 防止除零错误 */
static inline bool safe_divide(int32_t dividend,
                                int32_t divisor,
                                int32_t *result) {
    if (NULL == result) {
        return false;
    }

    if (0 == divisor) {
        return false;  /* 除零保护 */
    }

    if ((INT32_MIN == dividend) && (-1 == divisor)) {
        return false;  /* 溢出保护 */
    }

    *result = dividend / divisor;
    return true;
}

/* 安全数组访问 - 边界检查 */
#define SAFE_ARRAY_ACCESS(array, index, size, out_value) \
    do { \
        if (((index) < (size)) && ((index) >= 0)) { \
            (out_value) = (array)[index]; \
        } else { \
            (out_value) = 0; /* 默认值 */ \
        } \
    } while(0)
