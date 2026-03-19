/*
 * Auto-generated from: 07_Modern_Toolchain\13_Comprehensive_Examples\README.md
 * Line: 759
 * Language: c
 * Block ID: 7d3f7fc7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* high_perf_log.c - 核心实现 */
#include "high_perf_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdatomic.h>

/* 环形缓冲区 */
typedef struct {
    uint8_t *buffer;
    size_t size;
    _Atomic size_t write_pos;
    _Atomic size_t read_pos;
} ring_buffer_t;

struct logger {
    logger_config_t config;
    ring_buffer_t ring_buf;
    pthread_t flush_thread;
    atomic_bool running;
    int fd;
    uint8_t *mmap_buffer;
    size_t mmap_size;
    pthread_mutex_t file_lock;
};

/* 无锁写入环形缓冲区 */
static bool ring_buffer_write(ring_buffer_t *rb, const uint8_t *data, size_t len) {
    const size_t mask = rb->size - 1;
    size_t write_pos = atomic_load(&rb->write_pos);
    size_t read_pos = atomic_load(&rb->read_pos);

    /* 检查空间 */
    if ((write_pos - read_pos) > (rb->size - len)) {
        return false;  /* 缓冲区满 */
    }

    /* 写入数据 (可能分两次) */
    for (size_t i = 0; i < len; i++) {
        rb->buffer[(write_pos + i) & mask] = data[i];
    }

    /* 更新写指针 */
    atomic_store(&rb->write_pos, write_pos + len);

    return true;
}

/* 批量刷新线程 */
static void *flush_thread_func(void *arg) {
    logger_t *logger = (logger_t *)arg;

    while (atomic_load(&logger->running)) {
        logger_flush(logger);
        usleep(logger->config.flush_interval_ms * 1000);
    }

    /* 最终刷新 */
    logger_flush(logger);
    return NULL;
}

void logger_log(logger_t *logger, log_level_t level,
                const char *file, int line,
                const char *fmt, ...) {
    if (!logger || level < logger->config.level) return;

    /* 格式化消息 */
    char message[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(message, sizeof(message), fmt, args);
    va_end(args);

    /* 添加时间戳和元信息 */
    char full_message[2048];
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    const char *level_str[] = {"DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

    snprintf(full_message, sizeof(full_message),
             "[%04d-%02d-%02d %02d:%02d:%02d] [%s] %s:%d %s\n",
             tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday,
             tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec,
             level_str[level], file, line, message);

    size_t msg_len = strlen(full_message);

    if (logger->config.async_mode) {
        /* 异步模式: 写入环形缓冲区 */
        while (!ring_buffer_write(&logger->ring_buf,
                                  (uint8_t *)full_message, msg_len)) {
            /* 缓冲区满，等待刷新 */
            usleep(1000);
        }
    } else {
        /* 同步模式: 直接写入 */
        pthread_mutex_lock(&logger->file_lock);
        write(logger->fd, full_message, msg_len);
        pthread_mutex_unlock(&logger->file_lock);
    }
}

void logger_flush(logger_t *logger) {
    if (!logger || !logger->config.async_mode) return;

    ring_buffer_t *rb = &logger->ring_buf;
    const size_t mask = rb->size - 1;

    size_t read_pos = atomic_load(&rb->read_pos);
    size_t write_pos = atomic_load(&rb->write_pos);
    size_t available = write_pos - read_pos;

    if (available == 0) return;

    /* 批量写入文件 */
    pthread_mutex_lock(&logger->file_lock);

    while (available > 0) {
        size_t chunk = available;
        size_t offset = read_pos & mask;

        /* 处理回绕 */
        if (offset + chunk > rb->size) {
            chunk = rb->size - offset;
        }

        write(logger->fd, rb->buffer + offset, chunk);

        read_pos += chunk;
        available -= chunk;
    }

    fsync(logger->fd);
    pthread_mutex_unlock(&logger->file_lock);

    atomic_store(&rb->read_pos, read_pos);
}

logger_t *logger_create(const logger_config_t *config) {
    logger_t *logger = calloc(1, sizeof(logger_t));
    if (!logger) return NULL;

    logger->config = *config;
    atomic_store(&logger->running, true);
    pthread_mutex_init(&logger->file_lock, NULL);

    /* 打开日志文件 */
    logger->fd = open(config->filename,
                      O_WRONLY | O_CREAT | O_APPEND,
                      0644);
    if (logger->fd < 0) {
        free(logger);
        return NULL;
    }

    if (config->async_mode) {
        /* 初始化环形缓冲区 */
        logger->ring_buf.size = config->buffer_size;
        logger->ring_buf.buffer = aligned_alloc(64, config->buffer_size);
        atomic_init(&logger->ring_buf.write_pos, 0);
        atomic_init(&logger->ring_buf.read_pos, 0);

        /* 启动刷新线程 */
        pthread_create(&logger->flush_thread, NULL, flush_thread_func, logger);
    }

    return logger;
}

void logger_destroy(logger_t *logger) {
    if (!logger) return;

    atomic_store(&logger->running, false);

    if (logger->config.async_mode) {
        pthread_join(logger->flush_thread, NULL);
        free(logger->ring_buf.buffer);
    }

    close(logger->fd);
    pthread_mutex_destroy(&logger->file_lock);
    free(logger);
}
