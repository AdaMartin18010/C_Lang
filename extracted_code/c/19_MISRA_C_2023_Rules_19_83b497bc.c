/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\19_MISRA_C_2023_Rules_19.md
 * Line: 200
 * Language: c
 * Block ID: 83b497bc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 配置文件示例：config.h */

#ifndef CONFIG_H
#define CONFIG_H

/* 调试级别 */
#define DEBUG_LEVEL_NONE    0
#define DEBUG_LEVEL_ERROR   1
#define DEBUG_LEVEL_WARN    2
#define DEBUG_LEVEL_INFO    3

#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL DEBUG_LEVEL_ERROR
#endif

/* 功能开关 */
#define ENABLE_FEATURE_A 1
#define ENABLE_FEATURE_B 0

/* 平台抽象 */
#if defined(_WIN32)
    #define PLATFORM_PATH_SEP '\\'
#elif defined(__unix__) || defined(__APPLE__)
    #define PLATFORM_PATH_SEP '/'
#else
    #define PLATFORM_PATH_SEP '/'
#endif

/* 编译器特性 */
#if defined(__GNUC__) || defined(__clang__)
    #define UNUSED __attribute__((unused))
    #define NORETURN __attribute__((noreturn))
#elif defined(_MSC_VER)
    #define UNUSED
    #define NORETURN __declspec(noreturn)
#else
    #define UNUSED
    #define NORETURN
#endif

#endif /* CONFIG_H */
