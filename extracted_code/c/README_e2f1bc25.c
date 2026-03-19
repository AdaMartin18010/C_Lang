/*
 * Auto-generated from: 03_System_Technology_Domains\09_High_Performance_Log\README.md
 * Line: 316
 * Language: c
 * Block ID: e2f1bc25
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
