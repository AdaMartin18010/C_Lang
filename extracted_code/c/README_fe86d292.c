/*
 * Auto-generated from: 03_System_Technology_Domains\09_High_Performance_Log\README.md
 * Line: 62
 * Language: c
 * Block ID: fe86d292
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 基于 mmap 的零拷贝日志实现
 */

#define _GNU_SOURCE
#include <sys/mman.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdatomic.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>

#define LOG_FILE_SIZE (256 * 1024 * 1024)  // 256MB 日志文件
#define LOG_BUFFER_SIZE (4 * 1024 * 1024)   // 4MB 缓冲区

// 日志级别
typedef enum {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL,
} LogLevel;

static const char *level_strings[] = {
    "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
};

// 零拷贝日志上下文
typedef struct {
    int fd;                          // 日志文件描述符
    uint8_t *mmap_base;              // mmap 映射基地址
    size_t mmap_size;                // 映射大小
    _Atomic size_t write_offset;     // 当前写入位置（原子操作）
    size_t committed_offset;         // 已提交位置
    LogLevel min_level;              // 最小日志级别
    pthread_mutex_t commit_mutex;    // 提交锁
} ZeroCopyLogger;

// 初始化零拷贝日志系统
ZeroCopyLogger* zc_logger_init(const char *filename)
{
    ZeroCopyLogger *logger = calloc(1, sizeof(ZeroCopyLogger));
    if (!logger) return NULL;

    // 创建/打开日志文件
    logger->fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (logger->fd < 0) {
        free(logger);
        return NULL;
    }

    // 预分配文件空间
    if (ftruncate(logger->fd, LOG_FILE_SIZE) < 0) {
        close(logger->fd);
        free(logger);
        return NULL;
    }

    // mmap 映射文件
    logger->mmap_base = mmap(NULL, LOG_FILE_SIZE,
                              PROT_READ | PROT_WRITE,
                              MAP_SHARED, logger->fd, 0);
    if (logger->mmap_base == MAP_FAILED) {
        close(logger->fd);
        free(logger);
        return NULL;
    }

    // 建议内核使用顺序访问模式
    madvise(logger->mmap_base, LOG_FILE_SIZE, MADV_SEQUENTIAL);

    logger->mmap_size = LOG_FILE_SIZE;
    atomic_init(&logger->write_offset, 0);
    logger->committed_offset = 0;
    logger->min_level = LOG_LEVEL_INFO;
    pthread_mutex_init(&logger->commit_mutex, NULL);

    return logger;
}

// 直接写入 mmap 区域（零拷贝）
int zc_log_write(ZeroCopyLogger *logger, LogLevel level,
                  const char *file, int line, const char *fmt, ...)
{
    if (level < logger->min_level) return 0;

    // 获取当前时间
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    struct tm tm;
    localtime_r(&ts.tv_sec, &tm);

    // 预留写入位置（原子操作避免竞争）
    char stack_buffer[4096];
    int header_len = snprintf(stack_buffer, sizeof(stack_buffer),
        "[%04d-%02d-%02d %02d:%02d:%02d.%03d] [%s] [%s:%d] ",
        tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
        tm.tm_hour, tm.tm_min, tm.tm_sec, (int)(ts.tv_nsec / 1000000),
        level_strings[level], file, line);

    // 格式化消息
    va_list args;
    va_start(args, fmt);
    int msg_len = vsnprintf(stack_buffer + header_len,
                            sizeof(stack_buffer) - header_len, fmt, args);
    va_end(args);

    int total_len = header_len + msg_len;
    if (total_len > sizeof(stack_buffer) - 2) {
        total_len = sizeof(stack_buffer) - 2;
    }
    stack_buffer[total_len++] = '\n';
    stack_buffer[total_len] = '\0';

    // 原子分配空间
    size_t offset = atomic_fetch_add(&logger->write_offset, total_len);

    // 检查空间是否足够
    if (offset + total_len > logger->mmap_size) {
        // 处理日志回绕或扩容
        atomic_store(&logger->write_offset, logger->mmap_size);
        return -1;  // 日志已满
    }

    // 直接写入 mmap 区域（仅一次 memcpy）
    memcpy(logger->mmap_base + offset, stack_buffer, total_len);

    // 内存屏障确保写入可见
    __sync_synchronize();

    return total_len;
}

// 强制刷盘（msync）
void zc_logger_flush(ZeroCopyLogger *logger)
{
    size_t current = atomic_load(&logger->write_offset);
    size_t to_sync = current - logger->committed_offset;

    if (to_sync > 0) {
        msync(logger->mmap_base + logger->committed_offset,
              to_sync, MS_ASYNC);  // 异步刷盘
        logger->committed_offset = current;
    }
}

// 关闭日志系统
void zc_logger_close(ZeroCopyLogger *logger)
{
    if (!logger) return;

    // 最终同步
    msync(logger->mmap_base, logger->mmap_size, MS_SYNC);

    munmap(logger->mmap_base, logger->mmap_size);

    // 截断到实际大小
    size_t final_size = atomic_load(&logger->write_offset);
    ftruncate(logger->fd, final_size);

    close(logger->fd);
    pthread_mutex_destroy(&logger->commit_mutex);
    free(logger);
}
