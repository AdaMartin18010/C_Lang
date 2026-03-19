/*
 * Auto-generated from: 06_Thinking_Representation\02_Multidimensional_Matrix\01_Standards_Comparison_Matrix.md
 * Line: 2694
 * Language: c
 * Block ID: a0adf5fc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* portable_defs.h */
#ifndef PORTABLE_DEFS_H
#define PORTABLE_DEFS_H

#include "feature_detect.h"

/* 统一的关键字定义 */
#ifndef thread_local
    #if C11
        #define thread_local _Thread_local
    #elif COMPILER_GCC || COMPILER_CLANG
        #define thread_local __thread
    #elif COMPILER_MSVC
        #define thread_local __declspec(thread)
    #else
        #define thread_local
        #define NO_THREAD_LOCAL
    #endif
#endif

#ifndef alignas
    #if C11
        #define alignas _Alignas
    #elif COMPILER_GCC || COMPILER_CLANG
        #define alignas(x) __attribute__((aligned(x)))
    #elif COMPILER_MSVC
        #define alignas(x) __declspec(align(x))
    #else
        #define alignas(x)
        #define NO_ALIGNAS
    #endif
#endif

#ifndef alignof
    #if C11
        #define alignof _Alignof
    #elif COMPILER_GCC || COMPILER_CLANG
        #define alignof __alignof__
    #elif COMPILER_MSVC
        #define alignof __alignof
    #else
        #define alignof(x) sizeof(x)
    #endif
#endif

#ifndef noreturn
    #if C11
        #define noreturn _Noreturn
    #elif COMPILER_GCC || COMPILER_CLANG
        #define noreturn __attribute__((noreturn))
    #elif COMPILER_MSVC
        #define noreturn __declspec(noreturn)
    #else
        #define noreturn
    #endif
#endif

#ifndef static_assert
    #if C11
        #define static_assert _Static_assert
    #else
        /* 编译期断言的替代实现 */
        #define CONCAT_(a, b) a##b
        #define CONCAT(a, b) CONCAT_(a, b)
        #define static_assert(expr, msg) \
            typedef char CONCAT(static_assert_failed_, __LINE__)[(expr) ? 1 : -1]
    #endif
#endif

#ifndef nullptr
    #if C23
        /* 原生支持 */
    #elif defined(__cplusplus)
        #define nullptr NULL
    #else
        #define nullptr ((void*)0)
    #endif
#endif

/* 统一的属性语法 */
#if HAS_ATTRIBUTE_SYNTAX
    #define ATTR(x) [[x]]
#else
    #if COMPILER_GCC || COMPILER_CLANG
        #define ATTR(x) __attribute__((x))
    #elif COMPILER_MSVC
        #define ATTR(x) __declspec(x)
    #else
        #define ATTR(x)
    #endif
#endif

/* 具体属性 */
#define ATTR_NODISCARD ATTR(nodiscard)
#define ATTR_NORETURN ATTR(noreturn)
#define ATTR_MAYBE_UNUSED ATTR(maybe_unused)
#define ATTR_DEPRECATED(msg) ATTR(deprecated(msg))
#define ATTR_FALLTHROUGH ATTR(fallthrough)

#endif /* PORTABLE_DEFS_H */
