/*
 * Auto-generated from: 01_Core_Knowledge_System\03_Construction_Layer\02_Preprocessor.md
 * Line: 124
 * Language: c
 * Block ID: e77d8d2f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 功能检测（现代做法）
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
    #define COMPILER "GCC"
    #define COMPILER_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)
#elif defined(__clang__)
    #define COMPILER "Clang"
#elif defined(_MSC_VER)
    #define COMPILER "MSVC"
#endif

// 平台抽象
#if defined(_WIN32)
    #define PLATFORM "Windows"
    #ifdef _WIN64
        #define PLATFORM_64BIT
    #endif
#elif defined(__linux__)
    #define PLATFORM "Linux"
#elif defined(__APPLE__)
    #define PLATFORM "macOS"
#endif

// C标准版本检测
#if __STDC_VERSION__ >= 202311L
    #define C23
#elif __STDC_VERSION__ >= 201710L
    #define C17
#elif __STDC_VERSION__ >= 201112L
    #define C11
#elif __STDC_VERSION__ >= 199901L
    #define C99
#else
    #define C89
#endif
