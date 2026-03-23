# 高性能日志系统


---

## 📑 目录

- [高性能日志系统](#高性能日志系统)
  - [📑 目录](#-目录)
  - [概述](#概述)
  - [零拷贝日志](#零拷贝日志)
    - [零拷贝原理](#零拷贝原理)
    - [零拷贝日志实现](#零拷贝日志实现)
    - [使用 writev 的聚集写](#使用-writev-的聚集写)
  - [内存映射日志](#内存映射日志)
    - [多缓冲区 MMAP 设计](#多缓冲区-mmap-设计)
  - [异步日志](#异步日志)
    - [无锁环形缓冲区](#无锁环形缓冲区)
    - [异步日志架构](#异步日志架构)
  - [结构化日志](#结构化日志)
    - [JSON 结构化日志](#json-结构化日志)
    - [二进制结构化日志](#二进制结构化日志)
  - [性能优化与监控](#性能优化与监控)
  - [延伸阅读](#延伸阅读)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)


---

## 概述

高性能日志系统是现代软件基础设施的关键组件，直接影响系统的可观测性、故障排查效率和整体性能。本章节深入探讨零拷贝技术、内存映射日志、异步日志架构和结构化日志格式，为构建生产级日志系统提供全面的技术方案。

---

## 零拷贝日志

### 零拷贝原理

```
传统日志写入 vs 零拷贝日志：

传统方式（4 次拷贝）：
┌─────────────┐    ┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│  应用程序    │ -> │  用户缓冲区  │ -> │  内核缓冲区  │ -> │   磁盘       │
│  日志数据    │    │   (memcpy)  │    │  (page cache)│    │              │
└─────────────┘    └─────────────┘    └─────────────┘    └─────────────┘

零拷贝方式（1-2 次拷贝）：
┌─────────────┐                      ┌─────────────┐    ┌─────────────┐
│  用户空间    │  (直接访问页缓存)      │  内核页缓存   │ -> │   磁盘       │
│  环形缓冲区  │ ==================> │   (mmap)    │    │   (DMA)      │
└─────────────┘                      └─────────────┘    └─────────────┘

关键技术：
- mmap：用户空间直接访问内核页缓存
- writev：聚集写减少系统调用
- splice：管道零拷贝传输
```

### 零拷贝日志实现

```c
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
```

### 使用 writev 的聚集写

```c
/*
 * 基于 writev 的批量写入优化
 */

#include <sys/uio.h>
#include <stdarg.h>

#define MAX_IOVEC 64

typedef struct {
    struct iovec iovec[MAX_IOVEC];
    int count;
    size_t total_len;
} WriteBatch;

// 批量写入器
typedef struct {
    int fd;
    WriteBatch batch;
    pthread_mutex_t lock;
} BatchLogger;

// 添加日志到批次
int batch_log_add(BatchLogger *logger, const char *data, size_t len)
{
    pthread_mutex_lock(&logger->lock);

    if (logger->batch.count >= MAX_IOVEC) {
        // 批次已满，先写入
        batch_log_flush(logger);
    }

    logger->batch.iovec[logger->batch.count].iov_base = (void *)data;
    logger->batch.iovec[logger->batch.count].iov_len = len;
    logger->batch.count++;
    logger->batch.total_len += len;

    pthread_mutex_unlock(&logger->lock);
    return 0;
}

// 批量刷盘（单次系统调用写入多个缓冲区）
int batch_log_flush(BatchLogger *logger)
{
    if (logger->batch.count == 0) return 0;

    ssize_t written = writev(logger->fd, logger->batch.iovec,
                              logger->batch.count);

    // 重置批次
    logger->batch.count = 0;
    logger->batch.total_len = 0;

    return (written < 0) ? -1 : 0;
}

// 预分配和写入优化
void optimize_log_write(int fd, const void *data, size_t len)
{
    // 预分配文件空间（避免元数据更新开销）
    posix_fallocate(fd, 0, 1024 * 1024 * 100);  // 预分配 100MB

    // 设置写入提示
    posix_fadvise(fd, 0, 0, POSIX_FADV_SEQUENTIAL);

    // 使用 O_DIRECT 选项（绕过页缓存，直接写入磁盘）
    // int fd = open("log.txt", O_WRONLY | O_CREAT | O_DIRECT, 0644);

    // 使用 O_DSYNC 选项（仅同步数据，不同步元数据）
    // int fd = open("log.txt", O_WRONLY | O_CREAT | O_DSYNC, 0644);
}
```

---

## 内存映射日志

### 多缓冲区 MMAP 设计

```c
/*
 * 双缓冲区 MMAP 日志实现
 *
 * 使用两个 mmap 缓冲区交替写入，实现无锁切换
 */

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdatomic.h>

#define BUFFER_COUNT 2
#define BUFFER_SIZE (16 * 1024 * 1024)  // 16MB 每缓冲区

typedef struct {
    uint8_t *buffers[BUFFER_COUNT];    // 两个 mmap 缓冲区
    int current_buffer;                 // 当前写入缓冲区
    _Atomic size_t write_pos;           // 写入位置
    size_t file_offset;                 // 文件偏移
    int fd;
    pthread_mutex_t switch_lock;
} MmapLogger;

// 初始化双缓冲区日志
MmapLogger* mmap_logger_init(const char *filename)
{
    MmapLogger *logger = calloc(1, sizeof(MmapLogger));

    // 创建匿名 mmap 缓冲区（使用 MAP_ANONYMOUS）
    for (int i = 0; i < BUFFER_COUNT; i++) {
        logger->buffers[i] = mmap(NULL, BUFFER_SIZE,
                                   PROT_READ | PROT_WRITE,
                                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    }

    logger->fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
    logger->current_buffer = 0;
    atomic_init(&logger->write_pos, 0);
    pthread_mutex_init(&logger->switch_lock, NULL);

    return logger;
}

// 写入日志
int mmap_log_write(MmapLogger *logger, const void *data, size_t len)
{
    size_t pos = atomic_fetch_add(&logger->write_pos, len);

    // 检查当前缓冲区是否足够
    if (pos + len > BUFFER_SIZE) {
        // 触发缓冲区切换
        mmap_logger_switch_buffer(logger);
        pos = atomic_fetch_add(&logger->write_pos, len);
    }

    uint8_t *buf = logger->buffers[logger->current_buffer];
    memcpy(buf + pos, data, len);

    return len;
}

// 异步缓冲区切换
void mmap_logger_switch_buffer(MmapLogger *logger)
{
    pthread_mutex_lock(&logger->switch_lock);

    int prev_buffer = logger->current_buffer;
    size_t data_len = atomic_load(&logger->write_pos);

    // 切换到另一个缓冲区
    logger->current_buffer = (logger->current_buffer + 1) % BUFFER_COUNT;
    atomic_store(&logger->write_pos, 0);

    pthread_mutex_unlock(&logger->switch_lock);

    // 异步写入前一个缓冲区（可以在后台线程执行）
    pwrite(logger->fd, logger->buffers[prev_buffer], data_len,
           logger->file_offset);
    logger->file_offset += data_len;
}
```

---

## 异步日志

### 无锁环形缓冲区

```c
/*
 * 基于无锁环形缓冲区的异步日志
 *
 * 生产者（业务线程）-> 环形缓冲区 -> 消费者（日志线程）-> 磁盘
 */

#include <stdatomic.h>
#include <pthread.h>
#include <stdint.h>

#define RING_BUFFER_SIZE (64 * 1024 * 1024)  // 64MB 环形缓冲区
#define CACHE_LINE_SIZE 64

// 无锁环形缓冲区
typedef struct __attribute__((aligned(CACHE_LINE_SIZE))) {
    uint8_t *buffer;
    size_t size;

    // 使用缓存行对齐避免伪共享
    alignas(CACHE_LINE_SIZE) _Atomic size_t write_pos;
    alignas(CACHE_LINE_SIZE) _Atomic size_t read_pos;

    pthread_mutex_t notify_mutex;
    pthread_cond_t notify_cond;
} LockFreeRingBuffer;

// 初始化环形缓冲区
LockFreeRingBuffer* ring_buffer_create(size_t size)
{
    LockFreeRingBuffer *rb = aligned_alloc(CACHE_LINE_SIZE,
                                            sizeof(LockFreeRingBuffer));
    rb->buffer = aligned_alloc(CACHE_LINE_SIZE, size);
    rb->size = size;
    atomic_init(&rb->write_pos, 0);
    atomic_init(&rb->read_pos, 0);
    pthread_mutex_init(&rb->notify_mutex, NULL);
    pthread_cond_init(&rb->notify_cond, NULL);
    return rb;
}

// 生产者写入（无锁）
size_t ring_buffer_write(LockFreeRingBuffer *rb, const void *data,
                          size_t len)
{
    size_t write_pos = atomic_load(&rb->write_pos);
    size_t read_pos = atomic_load(&rb->read_pos);

    // 计算可用空间
    size_t available = rb->size - (write_pos - read_pos);
    if (len > available) {
        return 0;  // 缓冲区已满
    }

    // 计算实际写入位置（取模）
    size_t actual_pos = write_pos & (rb->size - 1);  // 要求 size 是 2 的幂

    // 处理环绕
    size_t first_chunk = rb->size - actual_pos;
    if (len <= first_chunk) {
        memcpy(rb->buffer + actual_pos, data, len);
    } else {
        memcpy(rb->buffer + actual_pos, data, first_chunk);
        memcpy(rb->buffer, (uint8_t *)data + first_chunk, len - first_chunk);
    }

    // 内存屏障 + 更新写位置
    __atomic_thread_fence(__ATOMIC_RELEASE);
    atomic_store(&rb->write_pos, write_pos + len);

    // 通知消费者
    pthread_cond_signal(&rb->notify_cond);

    return len;
}

// 消费者读取
size_t ring_buffer_read(LockFreeRingBuffer *rb, void *data, size_t len)
{
    size_t read_pos = atomic_load(&rb->read_pos);
    size_t write_pos = atomic_load(&rb->write_pos);

    size_t available = write_pos - read_pos;
    size_t to_read = (len < available) ? len : available;

    if (to_read == 0) return 0;

    size_t actual_pos = read_pos & (rb->size - 1);
    size_t first_chunk = rb->size - actual_pos;

    if (to_read <= first_chunk) {
        memcpy(data, rb->buffer + actual_pos, to_read);
    } else {
        memcpy(data, rb->buffer + actual_pos, first_chunk);
        memcpy((uint8_t *)data + first_chunk, rb->buffer,
               to_read - first_chunk);
    }

    __atomic_thread_fence(__ATOMIC_ACQUIRE);
    atomic_store(&rb->read_pos, read_pos + to_read);

    return to_read;
}
```

### 异步日志架构

```c
/*
 * 完整的异步日志系统
 */

#include <pthread.h>
#include <signal.h>

typedef struct {
    LockFreeRingBuffer *ring_buffer;
    pthread_t writer_thread;
    int log_fd;
    _Atomic int running;
    void (*flush_callback)(void);
} AsyncLogger;

// 日志线程主循环
void* log_writer_thread(void *arg)
{
    AsyncLogger *logger = arg;
    char read_buffer[256 * 1024];  // 256KB 读取缓冲区

    while (atomic_load(&logger->running)) {
        // 批量读取
        size_t bytes = ring_buffer_read(logger->ring_buffer,
                                         read_buffer,
                                         sizeof(read_buffer));

        if (bytes > 0) {
            // 批量写入文件
            write(logger->log_fd, read_buffer, bytes);
        } else {
            // 缓冲区为空，等待通知或超时
            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_nsec += 100 * 1000000;  // 100ms 超时
            pthread_cond_timedwait(&logger->ring_buffer->notify_cond,
                                   &logger->ring_buffer->notify_mutex,
                                   &ts);
        }

        // 定期刷盘（可选）
        static int flush_counter = 0;
        if (++flush_counter >= 10) {
            fdatasync(logger->log_fd);
            flush_counter = 0;
        }
    }

    // 退出前清空缓冲区
    size_t bytes;
    while ((bytes = ring_buffer_read(logger->ring_buffer,
                                      read_buffer,
                                      sizeof(read_buffer))) > 0) {
        write(logger->log_fd, read_buffer, bytes);
    }

    return NULL;
}

// 初始化异步日志
AsyncLogger* async_logger_init(const char *filename)
{
    AsyncLogger *logger = calloc(1, sizeof(AsyncLogger));
    logger->ring_buffer = ring_buffer_create(RING_BUFFER_SIZE);
    logger->log_fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);

    // 优化文件描述符
    fcntl(logger->log_fd, F_SETFL, O_DIRECT | O_APPEND);

    atomic_init(&logger->running, 1);
    pthread_create(&logger->writer_thread, NULL, log_writer_thread, logger);

    return logger;
}

// 前端写入接口（线程安全，无锁）
int async_log(AsyncLogger *logger, const char *fmt, ...)
{
    char buffer[4096];
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    // 添加换行符
    if (len < sizeof(buffer) - 1) {
        buffer[len++] = '\n';
    }

    return ring_buffer_write(logger->ring_buffer, buffer, len);
}

// 优雅关闭
void async_logger_shutdown(AsyncLogger *logger)
{
    atomic_store(&logger->running, 0);
    pthread_join(logger->writer_thread, NULL);
    close(logger->log_fd);
    free(logger);
}
```

---

## 结构化日志

### JSON 结构化日志

```c
/*
 * JSON 结构化日志实现
 *
 * 每条日志都是一个 JSON 对象，便于机器解析和分析
 */

#include <stdio.h>
#include <time.h>
#include <string.h>

typedef struct {
    const char *key;
    const char *value;
} LogField;

typedef struct {
    time_t timestamp;
    LogLevel level;
    const char *message;
    const char *service;
    const char *trace_id;
    LogField *fields;
    int field_count;
} StructuredLogEntry;

// 安全的 JSON 字符串转义
void json_escape(char *dest, size_t dest_size, const char *src)
{
    size_t j = 0;
    for (size_t i = 0; src[i] && j < dest_size - 1; i++) {
        switch (src[i]) {
            case '"':  if (j + 2 < dest_size) { dest[j++] = '\\'; dest[j++] = '"'; } break;
            case '\\': if (j + 2 < dest_size) { dest[j++] = '\\'; dest[j++] = '\\'; } break;
            case '\n': if (j + 2 < dest_size) { dest[j++] = '\\'; dest[j++] = 'n'; } break;
            case '\r': if (j + 2 < dest_size) { dest[j++] = '\\'; dest[j++] = 'r'; } break;
            case '\t': if (j + 2 < dest_size) { dest[j++] = '\\'; dest[j++] = 't'; } break;
            default:   dest[j++] = src[i]; break;
        }
    }
    dest[j] = '\0';
}

// 格式化结构化日志
void format_structured_log(char *output, size_t size,
                            const StructuredLogEntry *entry)
{
    char timestamp[64];
    struct tm tm;
    localtime_r(&entry->timestamp, &tm);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S%z", &tm);

    char escaped_message[2048];
    json_escape(escaped_message, sizeof(escaped_message), entry->message);

    int pos = snprintf(output, size,
        "{"
        "\"timestamp\":\"%s\","
        "\"level\":\"%s\","
        "\"message\":\"%s\","
        "\"service\":\"%s\","
        "\"trace_id\":\"%s\"",
        timestamp,
        level_strings[entry->level],
        escaped_message,
        entry->service ? entry->service : "unknown",
        entry->trace_id ? entry->trace_id : ""
    );

    // 添加额外字段
    for (int i = 0; i < entry->field_count && pos < (int)size - 1; i++) {
        char escaped_value[1024];
        json_escape(escaped_value, sizeof(escaped_value),
                    entry->fields[i].value);
        pos += snprintf(output + pos, size - pos,
            ",\"%s\":\"%s\"",
            entry->fields[i].key, escaped_value);
    }

    if (pos < (int)size - 1) {
        strcat(output + pos, "}\n");
    }
}

// 使用示例
void log_user_login(const char *user_id, const char *ip_address,
                     bool success)
{
    StructuredLogEntry entry = {
        .timestamp = time(NULL),
        .level = LOG_LEVEL_INFO,
        .message = "User login attempt",
        .service = "auth-service",
        .trace_id = "abc-123-def",
    };

    LogField fields[] = {
        {"user_id", user_id},
        {"ip_address", ip_address},
        {"success", success ? "true" : "false"},
        {"attempt_count", "3"},
    };
    entry.fields = fields;
    entry.field_count = 4;

    char json_output[4096];
    format_structured_log(json_output, sizeof(json_output), &entry);

    // 写入日志
    async_log(logger, "%s", json_output);
}
```

### 二进制结构化日志

```c
/*
 * 二进制结构化日志（更高效的存储和解析）
 */

#include <stdint.h>
#include <arpa/inet.h>

#define LOG_MAGIC 0x4C4F4701  // "LOG" + 版本 1

// 字段类型枚举
typedef enum {
    FIELD_TYPE_INT8 = 1,
    FIELD_TYPE_INT16,
    FIELD_TYPE_INT32,
    FIELD_TYPE_INT64,
    FIELD_TYPE_FLOAT,
    FIELD_TYPE_DOUBLE,
    FIELD_TYPE_STRING,
    FIELD_TYPE_BOOL,
    FIELD_TYPE_TIMESTAMP,
} FieldType;

// 二进制日志头部
typedef struct __attribute__((packed)) {
    uint32_t magic;           // 魔数
    uint32_t entry_length;    // 条目总长度
    uint64_t timestamp_ns;    // 纳秒时间戳
    uint8_t level;            // 日志级别
    uint16_t message_len;     // 消息长度
    uint16_t field_count;     // 字段数量
    // 后面跟着：消息字符串 + 字段数组
} BinaryLogHeader;

// 字段定义
typedef struct __attribute__((packed)) {
    uint8_t type;             // FieldType
    uint8_t name_len;         // 字段名长度
    uint16_t value_len;       // 值长度
    // 后面跟着：name（字符串）+ value
} BinaryLogField;

// 序列化结构化日志
size_t serialize_binary_log(uint8_t *buffer, size_t buffer_size,
                             const StructuredLogEntry *entry)
{
    if (buffer_size < sizeof(BinaryLogHeader)) return 0;

    BinaryLogHeader *header = (BinaryLogHeader *)buffer;
    header->magic = htonl(LOG_MAGIC);
    header->timestamp_ns = htobe64(entry->timestamp * 1000000000LL);
    header->level = entry->level;
    header->field_count = htons(entry->field_count);

    size_t pos = sizeof(BinaryLogHeader);

    // 写入消息
    header->message_len = htons(strlen(entry->message));
    memcpy(buffer + pos, entry->message, ntohs(header->message_len));
    pos += ntohs(header->message_len);

    // 写入字段
    for (int i = 0; i < entry->field_count; i++) {
        BinaryLogField field;
        field.type = FIELD_TYPE_STRING;  // 简化处理
        field.name_len = strlen(entry->fields[i].key);
        field.value_len = htons(strlen(entry->fields[i].value));

        memcpy(buffer + pos, &field, sizeof(field));
        pos += sizeof(field);

        memcpy(buffer + pos, entry->fields[i].key, field.name_len);
        pos += field.name_len;

        memcpy(buffer + pos, entry->fields[i].value,
               ntohs(field.value_len));
        pos += ntohs(field.value_len);
    }

    header->entry_length = htonl(pos);
    return pos;
}
```

---

## 性能优化与监控

```c
/*
 * 日志系统性能监控
 */

typedef struct {
    _Atomic uint64_t messages_logged;
    _Atomic uint64_t bytes_written;
    _Atomic uint64_t dropped_messages;
    _Atomic uint64_t write_latency_us;
    _Atomic uint64_t buffer_full_events;

    // 每秒统计
    uint64_t last_second_messages;
    uint64_t last_check_time;
} LogMetrics;

// 记录指标
void record_log_metrics(LogMetrics *metrics, size_t bytes,
                         uint64_t latency_us)
{
    atomic_fetch_add(&metrics->messages_logged, 1);
    atomic_fetch_add(&metrics->bytes_written, bytes);
    atomic_fetch_add(&metrics->write_latency_us, latency_us);
}

// 获取每秒日志速率
uint64_t get_logs_per_second(LogMetrics *metrics)
{
    uint64_t now = get_timestamp_us();
    uint64_t current = atomic_load(&metrics->messages_logged);

    uint64_t elapsed = now - metrics->last_check_time;
    uint64_t diff = current - metrics->last_second_messages;

    if (elapsed > 0) {
        return (diff * 1000000) / elapsed;
    }
    return 0;
}
```

---

## 延伸阅读

- [Linux 高性能日志](https://en.wikipedia.org/wiki/Syslog)
- [零拷贝技术](https://en.wikipedia.org/wiki/Zero-copy)
- [结构化日志最佳实践](https://stackify.com/what-is-structured-logging/)


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
