/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\RPi_Pico\01_Pico_Complete_Guide.md
 * Line: 3813
 * Language: c
 * Block ID: 43449cab
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file debug_utils.c
 * @brief 调试工具集
 */

#include "pico/stdlib.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

// 调试级别
typedef enum {
    DEBUG_LEVEL_NONE = 0,
    DEBUG_LEVEL_ERROR,
    DEBUG_LEVEL_WARN,
    DEBUG_LEVEL_INFO,
    DEBUG_LEVEL_DEBUG,
    DEBUG_LEVEL_VERBOSE
} debug_level_t;

static debug_level_t current_level = DEBUG_LEVEL_DEBUG;

void debug_printf(debug_level_t level, const char* file, int line,
                  const char* func, const char* fmt, ...) {
    if (level > current_level) return;

    const char* level_str[] = {"", "ERR", "WRN", "INF", "DBG", "VRB"};

    // 打印前缀
    printf("[%s] %s:%d %s: ", level_str[level], file, line, func);

    // 打印消息
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    printf("\n");
}

#define DEBUG_ERROR(...) \
    debug_printf(DEBUG_LEVEL_ERROR, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define DEBUG_WARN(...) \
    debug_printf(DEBUG_LEVEL_WARN, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define DEBUG_INFO(...) \
    debug_printf(DEBUG_LEVEL_INFO, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define DEBUG_DEBUG(...) \
    debug_printf(DEBUG_LEVEL_DEBUG, __FILE__, __LINE__, __func__, __VA_ARGS__)

// 内存转储
void hexdump(const void* data, size_t size) {
    const uint8_t* p = (const uint8_t*)data;

    for (size_t i = 0; i < size; i += 16) {
        printf("%04X: ", (unsigned int)i);

        // 十六进制
        for (size_t j = 0; j < 16 && i + j < size; j++) {
            printf("%02X ", p[i + j]);
        }

        // 对齐
        for (size_t j = (size - i < 16) ? size - i : 16; j < 16; j++) {
            printf("   ");
        }

        printf(" ");

        // ASCII
        for (size_t j = 0; j < 16 && i + j < size; j++) {
            uint8_t c = p[i + j];
            printf("%c", (c >= 32 && c < 127) ? c : '.');
        }

        printf("\n");
    }
}

// 性能计时器
typedef struct {
    absolute_time_t start;
    const char* name;
} perf_timer_t;

perf_timer_t perf_start(const char* name) {
    perf_timer_t timer = {
        .start = get_absolute_time(),
        .name = name
    };
    return timer;
}

void perf_end(perf_timer_t* timer) {
    int64_t elapsed = absolute_time_diff_us(timer->start, get_absolute_time());
    DEBUG_DEBUG("%s: %lld us", timer->name, elapsed);
}

#define PERF_SCOPE(name) \
    perf_timer_t _perf_timer __attribute__((cleanup(perf_end_cleanup))) = perf_start(name)

static void perf_end_cleanup(perf_timer_t* timer) {
    perf_end(timer);
}
