/*
 * Auto-generated from: 07_Modern_Toolchain\13_Comprehensive_Examples\README.md
 * Line: 708
 * Language: c
 * Block ID: a2914cc2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* high_perf_log.h */
#ifndef HIGH_PERF_LOG_H
#define HIGH_PERF_LOG_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL
} log_level_t;

typedef struct logger logger_t;

/* 配置 */
typedef struct {
    const char *filename;
    log_level_t level;
    uint32_t buffer_size;       /* 缓冲区大小 */
    uint32_t flush_interval_ms; /* 自动刷新间隔 */
    bool async_mode;            /* 异步模式 */
    bool compress;              /* 启用压缩 */
} logger_config_t;

/* API */
logger_t *logger_create(const logger_config_t *config);
void logger_destroy(logger_t *logger);

void logger_log(logger_t *logger, log_level_t level,
                const char *file, int line,
                const char *fmt, ...);

void logger_flush(logger_t *logger);

/* 宏定义 */
#define LOG_DEBUG(logger, ...) \
    logger_log(logger, LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(logger, ...) \
    logger_log(logger, LOG_LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARN(logger, ...) \
    logger_log(logger, LOG_LEVEL_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(logger, ...) \
    logger_log(logger, LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)

#endif /* HIGH_PERF_LOG_H */
