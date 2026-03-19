/*
 * Auto-generated from: 10_WebAssembly_C\02_Emscripten_Advanced_Usage.md
 * Line: 3270
 * Language: c
 * Block ID: 10668557
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// debug_utils.c
#include <emscripten/emscripten.h>
#include <stdio.h>
#include <stdarg.h>

// 调试日志级别
typedef enum {
    LOG_DEBUG = 0,
    LOG_INFO = 1,
    LOG_WARN = 2,
    LOG_ERROR = 3
} LogLevel;

static LogLevel current_log_level = LOG_DEBUG;

EMSCRIPTEN_KEEPALIVE
void set_log_level(LogLevel level) {
    current_log_level = level;
}

void log_message(LogLevel level, const char* file, int line, const char* fmt, ...) {
    if (level < current_log_level) return;

    const char* level_str[] = {"DEBUG", "INFO", "WARN", "ERROR"};

    fprintf(stderr, "[%s] %s:%d: ", level_str[level], file, line);

    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fprintf(stderr, "\n");
}

#define LOG_DEBUG(...) log_message(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...)  log_message(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARN(...)  log_message(LOG_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) log_message(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)

// 断言宏
#define WASM_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            LOG_ERROR("Assertion failed: %s\n  Condition: %s\n  File: %s:%d", \
                      message, #condition, __FILE__, __LINE__); \
            emscripten_debugger(); \
        } \
    } while(0)

// 内存调试
EMSCRIPTEN_KEEPALIVE
void print_memory_stats() {
    LOG_INFO("Memory stats:");
    LOG_INFO("  HEAP8 length: %zu", (size_t)EM_ASM_INT({ return Module.HEAP8.length; }));
    LOG_INFO("  Total memory: %zu MB",
             (size_t)EM_ASM_INT({ return Module.buffer.byteLength; }) / (1024 * 1024));
}

// 性能计时器
typedef struct {
    double start_time;
    const char* name;
} Timer;

EMSCRIPTEN_KEEPALIVE
Timer* timer_start(const char* name) {
    Timer* timer = (Timer*)malloc(sizeof(Timer));
    timer->name = name;
    timer->start_time = emscripten_get_now();
    return timer;
}

EMSCRIPTEN_KEEPALIVE
void timer_end(Timer* timer) {
    double elapsed = emscripten_get_now() - timer->start_time;
    LOG_INFO("Timer [%s]: %.3f ms", timer->name, elapsed);
    free(timer);
}
