/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\19_MISRA_C_2023_Rules_19.md
 * Line: 153
 * Language: c
 * Block ID: 2f411a41
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 完整的条件编译 */
#ifdef FEATURE_X
    /* 代码 */
#elif defined(FEATURE_Y)
    /* 代码 */
#else
    /* 默认代码 */
#endif

/* ✅ 合规 - 头文件保护 */
#ifndef UNIQUE_HEADER_H
#define UNIQUE_HEADER_H

/* 内容 */

#endif /* UNIQUE_HEADER_H */

/* ✅ 合规 - 平台检测 */
#if defined(_WIN32)
    /* Windows代码 */
#elif defined(__linux__)
    /* Linux代码 */
#elif defined(__APPLE__)
    /* macOS代码 */
#else
    #error "Unsupported platform"
#endif

/* ✅ 合规 - 版本检查 */
#if __STDC_VERSION__ >= 201112L
    /* C11代码 */
#elif __STDC_VERSION__ >= 199901L
    /* C99代码 */
#else
    /* C89代码 */
#endif

/* ✅ 合规 - 静态断言 */
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
    #include <assert.h>
    static_assert(sizeof(int) == 4, "int must be 32-bit");
#endif
