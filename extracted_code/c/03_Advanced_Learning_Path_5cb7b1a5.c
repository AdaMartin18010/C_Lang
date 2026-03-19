/*
 * Auto-generated from: 06_Thinking_Representation\06_Learning_Paths\03_Advanced_Learning_Path.md
 * Line: 356
 * Language: c
 * Block ID: 5cb7b1a5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#ifndef HIGH_PERF_LOG_H
#define HIGH_PERF_LOG_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

// 日志级别
typedef enum {
    LOG_LEVEL_TRACE = 0,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL
} LogLevel;

// 配置
typedef struct {
    LogLevel level;
    const char *file_path;
    size_t max_file_size;
    size_t max_files;        // 轮转文件数
    bool async_mode;         // 异步模式
    size_t queue_size;       // 异步队列大小
    bool compress_old;       // 压缩旧日志
} LogConfig;

// API
int log_init(const LogConfig *config);
void log_shutdown(void);

// 格式化日志（使用宏减少开销）
#define LOG_TRACE(fmt, ...) \
    log_write(LOG_LEVEL_TRACE, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) \
    log_write(LOG_LEVEL_DEBUG, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) \
    log_write(LOG_LEVEL_INFO, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) \
    log_write(LOG_LEVEL_ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

void log_write(LogLevel level, const char *file, int line,
               const char *fmt, ...);

// 性能目标：单线程 > 1GB/s，异步 > 2GB/s
#endif
