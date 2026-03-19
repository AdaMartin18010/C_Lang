/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\07_MISRA_C_2023_Rules_7.md
 * Line: 140
 * Language: c
 * Block ID: 1f49164d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#ifndef SAFE_POINTER_H
#define SAFE_POINTER_H

#include <stddef.h>

/* 断言式检查 */
#define ASSERT_NOT_NULL(ptr) \
    do { \
        if ((ptr) == NULL) { \
            /* 记录错误、触发断言或返回 */ \
            return; \
        } \
    } while(0)

/* 带返回值的检查 */
#define SAFE_DEREF(ptr, default_val) \
    ((ptr) != NULL ? *(ptr) : (default_val))

/* 检查并执行 */
#define IF_NOT_NULL(ptr, action) \
    do { \
        if ((ptr) != NULL) { \
            action; \
        } \
    } while(0)

#endif
