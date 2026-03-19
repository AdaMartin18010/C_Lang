/*
 * Auto-generated from: 00_VERSION_TRACKING\GCC_15_C23_Default.md
 * Line: 3038
 * Language: c
 * Block ID: 218ba90d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* compiler_compat.h - 编译器兼容性头文件 */
#ifndef COMPILER_COMPAT_H
#define COMPILER_COMPAT_H

/* 检测编译器 */
#if defined(__GNUC__)
    #define COMPILER_GCC 1
    #define COMPILER_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#elif defined(__clang__)
    #define COMPILER_CLANG 1
    #define COMPILER_VERSION (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__)
#elif defined(_MSC_VER)
    #define COMPILER_MSVC 1
    #define COMPILER_VERSION _MSC_VER
#else
    #define COMPILER_UNKNOWN 1
    #define COMPILER_VERSION 0
#endif

/* 检测 C 标准 */
#ifdef __STDC_VERSION__
    #if __STDC_VERSION__ >= 202311L
        #define C23_SUPPORTED 1
        #define C23_ATTRIBUTES 1
    #elif __STDC_VERSION__ >= 201710L
        #define C17_SUPPORTED 1
    #elif __STDC_VERSION__ >= 201112L
        #define C11_SUPPORTED 1
    #endif
#else
    #define C89_SUPPORTED 1
#endif

/* 属性适配 */
#if C23_SUPPORTED && defined(__has_c_attribute)
    /* C23 原生属性 */
    #if __has_c_attribute(nodiscard)
        #define ATTR_NODISCARD [[nodiscard]]
    #else
        #define ATTR_NODISCARD
    #endif

    #if __has_c_attribute(maybe_unused)
        #define ATTR_MAYBE_UNUSED [[maybe_unused]]
    #else
        #define ATTR_MAYBE_UNUSED
    #endif

    #if __has_c_attribute(deprecated)
        #define ATTR_DEPRECATED [[deprecated]]
    #else
        #define ATTR_DEPRECATED
    #endif

    #if __has_c_attribute(fallthrough)
        #define ATTR_FALLTHROUGH [[fallthrough]]
    #else
        #define ATTR_FALLTHROUGH
    #endif
#else
    /* 回退到编译器特定扩展 */
    #if defined(COMPILER_GCC) || defined(COMPILER_CLANG)
        #define ATTR_NODISCARD __attribute__((warn_unused_result))
        #define ATTR_MAYBE_UNUSED __attribute__((unused))
        #define ATTR_DEPRECATED __attribute__((deprecated))
        #define ATTR_FALLTHROUGH __attribute__((fallthrough))
    #elif defined(COMPILER_MSVC)
        #define ATTR_NODISCARD _Check_return_
        #define ATTR_MAYBE_UNUSED
        #define ATTR_DEPRECATED __declspec(deprecated)
        #define ATTR_FALLTHROUGH
    #else
        #define ATTR_NODISCARD
        #define ATTR_MAYBE_UNUSED
        #define ATTR_DEPRECATED
        #define ATTR_FALLTHROUGH
    #endif
#endif

/* 关键字适配 */
#if C23_SUPPORTED
    /* C23: 原生关键字 */
    #define STATIC_ASSERT static_assert
#else
    #if defined(COMPILER_GCC) || defined(COMPILER_CLANG)
        #define STATIC_ASSERT _Static_assert
    #else
        #define STATIC_ASSERT(cond, msg) typedef int static_assert_##__LINE__[(cond) ? 1 : -1]
    #endif
#endif

/* nullptr 适配 */
#if C23_SUPPORTED
    /* 原生 nullptr */
#elif defined(__cplusplus)
    /* C++ nullptr */
#else
    #define nullptr ((void*)0)
#endif

/* thread_local 适配 */
#if C23_SUPPORTED
    /* 原生 thread_local */
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG)
    #define thread_local _Thread_local
#elif defined(COMPILER_MSVC)
    #define thread_local __declspec(thread)
#else
    #error "thread_local not supported"
#endif

/* 函数 */
ATTR_NODISCARD int example_function(void);

#endif /* COMPILER_COMPAT_H */
