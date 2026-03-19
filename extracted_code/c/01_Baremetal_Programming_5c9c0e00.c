/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Baremetal\01_Baremetal_Programming.md
 * Line: 4312
 * Language: c
 * Block ID: 5c9c0e00
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#ifndef DEBUG_H
#define DEBUG_H

#include <stdint.h>

/* 调试级别 */
#define DEBUG_LEVEL_NONE    0
#define DEBUG_LEVEL_ERROR   1
#define DEBUG_LEVEL_WARN    2
#define DEBUG_LEVEL_INFO    3
#define DEBUG_LEVEL_DEBUG   4

/* 设置当前调试级别 */
#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL DEBUG_LEVEL_INFO
#endif

/* 调试输出函数 */
extern void Debug_Print(const char *file, int line, const char *func,
                        const char *fmt, ...);

/* 条件编译的调试宏 */
#if DEBUG_LEVEL >= DEBUG_LEVEL_ERROR
    #define DEBUG_ERROR(fmt, ...) \
        Debug_Print(__FILE__, __LINE__, __func__, "[ERR] " fmt, ##__VA_ARGS__)
#else
    #define DEBUG_ERROR(fmt, ...) ((void)0)
#endif

#if DEBUG_LEVEL >= DEBUG_LEVEL_WARN
    #define DEBUG_WARN(fmt, ...) \
        Debug_Print(__FILE__, __LINE__, __func__, "[WRN] " fmt, ##__VA_ARGS__)
#else
    #define DEBUG_WARN(fmt, ...) ((void)0)
#endif

#if DEBUG_LEVEL >= DEBUG_LEVEL_INFO
    #define DEBUG_INFO(fmt, ...) \
        Debug_Print(__FILE__, __LINE__, __func__, "[INF] " fmt, ##__VA_ARGS__)
#else
    #define DEBUG_INFO(fmt, ...) ((void)0)
#endif

#if DEBUG_LEVEL >= DEBUG_LEVEL_DEBUG
    #define DEBUG_DEBUG(fmt, ...) \
        Debug_Print(__FILE__, __LINE__, __func__, "[DBG] " fmt, ##__VA_ARGS__)
#else
    #define DEBUG_DEBUG(fmt, ...) ((void)0)
#endif

/* 断言 */
#define ASSERT(cond) \
    do { \
        if (!(cond)) { \
            DEBUG_ERROR("Assertion failed: %s", #cond); \
            while (1); \
        } \
    } while (0)

/* 打印变量值 */
#define DEBUG_VAR(var) \
    DEBUG_DEBUG("%s = %d (0x%X)", #var, (int)(var), (unsigned int)(var))

/* 打印内存区域 */
void Debug_DumpMemory(const char *name, const void *addr, uint32_t len);

/* 打印调用栈 */
void Debug_PrintStackTrace(void);

#endif /* DEBUG_H */
