/*
 * Auto-generated from: 03_System_Technology_Domains\09_High_Performance_Log\README.md
 * Line: 512
 * Language: c
 * Block ID: fc579c20
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
