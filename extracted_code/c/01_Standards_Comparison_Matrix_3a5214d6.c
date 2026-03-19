/*
 * Auto-generated from: 06_Thinking_Representation\02_Multidimensional_Matrix\01_Standards_Comparison_Matrix.md
 * Line: 2580
 * Language: c
 * Block ID: 3a5214d6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* feature_detect.h */
#ifndef FEATURE_DETECT_H
#define FEATURE_DETECT_H

/* 编译器检测 */
#if defined(__GNUC__)
    #define COMPILER_GCC 1
    #define COMPILER_CLANG 0
    #define COMPILER_MSVC 0
#elif defined(__clang__)
    #define COMPILER_GCC 0
    #define COMPILER_CLANG 1
    #define COMPILER_MSVC 0
#elif defined(_MSC_VER)
    #define COMPILER_GCC 0
    #define COMPILER_CLANG 0
    #define COMPILER_MSVC 1
#else
    #define COMPILER_GCC 0
    #define COMPILER_CLANG 0
    #define COMPILER_MSVC 0
#endif

/* C标准版本检测 */
#if defined(__STDC_VERSION__)
    #if __STDC_VERSION__ >= 202311L
        #define C23 1
        #define C17 1
        #define C11 1
        #define C99 1
        #define C89 1
    #elif __STDC_VERSION__ >= 201710L
        #define C23 0
        #define C17 1
        #define C11 1
        #define C99 1
        #define C89 1
    #elif __STDC_VERSION__ >= 201112L
        #define C23 0
        #define C17 0
        #define C11 1
        #define C99 1
        #define C89 1
    #elif __STDC_VERSION__ >= 199901L
        #define C23 0
        #define C17 0
        #define C11 0
        #define C99 1
        #define C89 1
    #else
        #define C23 0
        #define C17 0
        #define C11 0
        #define C99 0
        #define C89 1
    #endif
#else
    #define C23 0
    #define C17 0
    #define C11 0
    #define C99 0
    #define C89 1
#endif

/* 特性可用性检测 */
#if C11
    #include <stdalign.h>
    #include <stdnoreturn.h>
    #define HAS_STATIC_ASSERT 1
    #define HAS_ALIGNAS 1
    #define HAS_NORETURN 1
#else
    #define HAS_STATIC_ASSERT 0
    #define HAS_ALIGNAS 0
    #define HAS_NORETURN 0
#endif

#if C23
    #define HAS_NULLPTR 1
    #define HAS_TYPEOF 1
    #define HAS_AUTO 1
    #define HAS_ATTRIBUTE_SYNTAX 1
#else
    #define HAS_NULLPTR 0
    #define HAS_TYPEOF 0
    #define HAS_AUTO 0
    #define HAS_ATTRIBUTE_SYNTAX 0
#endif

/* 可选特性检测 */
#if defined(__STDC_NO_ATOMICS__)
    #define HAS_ATOMICS 0
#else
    #define HAS_ATOMICS C11
#endif

#if defined(__STDC_NO_THREADS__)
    #define HAS_THREADS 0
#else
    #define HAS_THREADS C11
#endif

#if defined(__STDC_NO_VLA__)
    #define HAS_VLA 0
#else
    #define HAS_VLA C99
#endif

#endif /* FEATURE_DETECT_H */
