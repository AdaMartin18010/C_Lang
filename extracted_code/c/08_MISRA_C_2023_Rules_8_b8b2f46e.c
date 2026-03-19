/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\08_MISRA_C_2023_Rules_8.md
 * Line: 134
 * Language: c
 * Block ID: b8b2f46e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* MISRA合规的多语句宏 */
#define SAFE_FREE(ptr) do { \
    if ((ptr) != NULL) { \
        free(ptr); \
        (ptr) = NULL; \
    } \
} while(0)

/* 带返回值的宏 */
#define CHECK_NULL(ptr, ret) do { \
    if ((ptr) == NULL) { \
        return (ret); \
    } \
} while(0)

/* 使用 */
void *p = malloc(100);
SAFE_FREE(p);  /* 展开为安全的if语句 */
