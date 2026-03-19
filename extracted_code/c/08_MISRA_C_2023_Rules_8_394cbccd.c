/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\08_MISRA_C_2023_Rules_8.md
 * Line: 340
 * Language: c
 * Block ID: 394cbccd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 完整的条件 */
#ifdef FEATURE_ENABLED
    /* 特性代码 */
#else
    /* 默认代码 */
#endif

/* ✅ 合规 - 清晰的嵌套 */
#ifdef PLATFORM_LINUX
    #ifdef USE_EPOLL
        /* Linux epoll代码 */
    #else
        /* Linux select代码 */
    #endif
#elif defined(PLATFORM_WINDOWS)
    /* Windows代码 */
#else
    #error "Unsupported platform"
#endif

/* ✅ 合规 - 使用静态断言 */
#ifndef BUFFER_SIZE
    #error "BUFFER_SIZE must be defined"
#endif
