/*
 * Auto-generated from: 06_Thinking_Representation\02_Multidimensional_Matrix\01_Standards_Comparison_Matrix.md
 * Line: 2216
 * Language: c
 * Block ID: 702b5cb3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* compat.h - 多标准兼容层 */
#ifndef COMPAT_H
#define COMPAT_H

#include <stddef.h>

/* 检测C标准版本 */
#ifndef __STDC_VERSION__
    #define __STDC_VERSION__ 199409L  /* C89 */
#endif

/* nullptr 兼容 */
#if __STDC_VERSION__ >= 202311L
    /* C23: 原生支持 nullptr */
    #ifndef HAS_NATIVE_NULLPTR
        #define HAS_NATIVE_NULLPTR 1
    #endif
#elif defined(__cplusplus)
    /* C++11+: 使用 nullptr */
    #define nullptr NULL
#else
    /* C89-C17: 模拟 nullptr */
    #ifndef nullptr
        #define nullptr ((void*)0)
    #endif
    typedef void* nullptr_t;
#endif

/* bool 兼容 */
#if __STDC_VERSION__ >= 199901L
    #include <stdbool.h>
#else
    typedef enum { false, true } bool;
#endif

/* static_assert 兼容 */
#if __STDC_VERSION__ >= 201112L
    #define STATIC_ASSERT(cond, msg) _Static_assert(cond, msg)
#else
    /* C99/C89: 使用编译期断言技巧 */
    #define STATIC_ASSERT_1(cond, line) \
        typedef char static_assert_failed_##line[(cond) ? 1 : -1]
    #define STATIC_ASSERT_0(cond, line) STATIC_ASSERT_1(cond, line)
    #define STATIC_ASSERT(cond, msg) STATIC_ASSERT_0(cond, __LINE__)
#endif

/* thread_local 兼容 */
#if __STDC_VERSION__ >= 201112L
    #define THREAD_LOCAL _Thread_local
#elif defined(__GNUC__)
    #define THREAD_LOCAL __thread
#elif defined(_MSC_VER)
    #define THREAD_LOCAL __declspec(thread)
#else
    #define THREAD_LOCAL
#endif

/* noreturn 兼容 */
#if __STDC_VERSION__ >= 201112L
    #include <stdnoreturn.h>
#elif defined(__GNUC__)
    #define noreturn __attribute__((noreturn))
#elif defined(_MSC_VER)
    #define noreturn __declspec(noreturn)
#else
    #define noreturn
#endif

/* alignas/alignof 兼容 */
#if __STDC_VERSION__ >= 201112L
    #include <stdalign.h>
#elif defined(__GNUC__)
    #define alignas(n) __attribute__((aligned(n)))
    #define alignof(t) __alignof__(t)
#elif defined(_MSC_VER)
    #define alignas(n) __declspec(align(n))
    #define alignof(t) __alignof(t)
#else
    #define alignas(n)
    #define alignof(t) sizeof(t)
#endif

/* restrict 兼容 */
#if __STDC_VERSION__ >= 199901L
    /* C99+ 原生支持 */
#elif defined(__GNUC__)
    #define restrict __restrict
#elif defined(_MSC_VER)
    #define restrict __restrict
#else
    #define restrict
#endif

/* inline 兼容 */
#if __STDC_VERSION__ >= 199901L
    /* C99+ 原生支持 */
#elif defined(__GNUC__)
    #define inline __inline
#elif defined(_MSC_VER)
    #define inline __inline
#else
    #define inline
#endif

#endif /* COMPAT_H */
