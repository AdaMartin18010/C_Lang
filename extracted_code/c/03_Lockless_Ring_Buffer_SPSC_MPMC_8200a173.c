/*
 * Auto-generated from: 03_System_Technology_Domains\09_High_Performance_Log\03_Lockless_Ring_Buffer_SPSC_MPMC.md
 * Line: 269
 * Language: c
 * Block ID: 8200a173
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 高性能日志系统
typedef enum {
    LOG_DEBUG = 0,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL
} LogLevel;

typedef struct {
    uint64_t timestamp;
    LogLevel level;
    uint32_t tid;
    char message[240];  // 对齐到256字节
} LogEntry;

#define LOG_ENTRY_SIZE 256

typedef struct {
    RingBuffer *ring;
    FILE *output;
    _Atomic bool running;
    pthread_t writer_thread;
} Logger;

// 写线程
static void* log_writer_thread(void *arg) {
    Logger *logger = arg;
    LogEntry entry;

    while (atomic_load(&logger->running) ||
           ring_buffer_dequeue(logger->ring, &entry)) {
        if (ring_buffer_dequeue(logger->ring, &entry)) {
            // 格式化并写入
            char time_str[32];
            struct tm *tm = localtime((time_t*)&entry.timestamp);
            strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm);

            const char *level_str[] = {"DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

            fprintf(logger->output, "[%s] [%s] [TID:%u] %s\n",
                    time_str, level_str[entry.level], entry.tid, entry.message);
        }
    }

    return NULL;
}

// 异步日志（无锁，仅内存拷贝）
void log_async(Logger *logger, LogLevel level, const char *fmt, ...) {
    LogEntry entry;
    entry.timestamp = (uint64_t)time(NULL);
    entry.level = level;
    entry.tid = (uint32_t)pthread_self();

    va_list args;
    va_start(args, fmt);
    vsnprintf(entry.message, sizeof(entry.message), fmt, args);
    va_end(args);

    // 尝试入队，失败则丢弃或阻塞
    if (!ring_buffer_enqueue(logger->ring, &entry)) {
        // 队列满处理：可丢弃或扩容
        // 这里简单丢弃
    }
}
