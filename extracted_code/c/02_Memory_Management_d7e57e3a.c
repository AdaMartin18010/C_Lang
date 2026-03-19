/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\02_Memory_Management.md
 * Line: 2732
 * Language: c
 * Block ID: d7e57e3a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#ifndef SAFE_MEMORY_H
#define SAFE_MEMORY_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* 安全分配宏 - 自动检查NULL */
#define SAFE_MALLOC(ptr, type, count) do { \
    size_t _size = (size_t)(count) * sizeof(type); \
    if ((count) != 0 && _size / (count) != sizeof(type)) { \
        (ptr) = NULL; /* 溢出检测 */ \
    } else { \
        (ptr) = malloc(_size); \
    } \
} while(0)

#define SAFE_CALLOC(ptr, type, count) do { \
    (ptr) = calloc((size_t)(count), sizeof(type)); \
} while(0)

/* 安全释放宏 - 自动置NULL */
#define SAFE_FREE(ptr) do { \
    free(ptr); \
    (ptr) = NULL; \
} while(0)

/* 安全realloc - 不丢失原指针 */
#define SAFE_REALLOC(ptr, type, new_count) do { \
    size_t _new_size = (size_t)(new_count) * sizeof(type); \
    type *_new_ptr = realloc((ptr), _new_size); \
    if (_new_ptr != NULL || (new_count) == 0) { \
        (ptr) = _new_ptr; \
    } \
} while(0)

/* 数组访问边界检查（调试模式） */
#ifdef DEBUG
    #define ARRAY_ACCESS(arr, idx, size) \
        ((idx) < (size) ? &(arr)[idx] : \
         (fprintf(stderr, "Array bounds error: %s:%d idx=%zu size=%zu\n", \
                  __FILE__, __LINE__, (size_t)(idx), (size_t)(size)), \
          abort(), &(arr)[0]))
#else
    #define ARRAY_ACCESS(arr, idx, size) (&(arr)[idx])
#endif

/* 指针有效性断言 */
#ifdef DEBUG
    #define PTR_ASSERT(ptr) do { \
        if ((ptr) == NULL) { \
            fprintf(stderr, "NULL pointer: %s:%d\n", __FILE__, __LINE__); \
            abort(); \
        } \
    } while(0)
#else
    #define PTR_ASSERT(ptr) ((void)0)
#endif

/* 内存清零释放（敏感数据） */
#define SECURE_FREE(ptr, size) do { \
    if (ptr) { \
        memset((ptr), 0, (size)); \
        free(ptr); \
        (ptr) = NULL; \
    } \
} while(0)

#endif /* SAFE_MEMORY_H */
