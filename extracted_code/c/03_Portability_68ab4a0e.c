/*
 * Auto-generated from: 01_Core_Knowledge_System\06_Advanced_Layer\03_Portability.md
 * Line: 110
 * Language: c
 * Block ID: 68ab4a0e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 严格符合程序（Standard C）
// 1. 只使用标准定义的特性
// 2. 不依赖未定义、未指定、实现定义行为
// 3. 在翻译单元内独立（不依赖外部库）

// 检查标准版本
#if __STDC_VERSION__ >= 202311L
    // C23
#elif __STDC_VERSION__ >= 201710L
    // C17
#elif __STDC_VERSION__ >= 201112L
    // C11
#elif __STDC_VERSION__ >= 199901L
    // C99
#else
    // C89/C90
#endif

// 特性检测（现代方式）
#ifdef __has_feature
    #define HAS_FEATURE(x) __has_feature(x)
#else
    #define HAS_FEATURE(x) 0
#endif

#ifdef __has_include
    #define HAS_INCLUDE(x) __has_include(x)
#else
    #define HAS_INCLUDE(x) 0
#endif

// 编译器检测
#if defined(__GNUC__)
    #define COMPILER_GCC (__GNUC__ * 100 + __GNUC_MINOR__)
#elif defined(__clang__)
    #define COMPILER_CLANG (__clang_major__ * 100 + __clang_minor__)
#elif defined(_MSC_VER)
    #define COMPILER_MSVC _MSC_VER
#endif
