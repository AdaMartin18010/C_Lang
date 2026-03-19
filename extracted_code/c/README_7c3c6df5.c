/*
 * Auto-generated from: 03_System_Technology_Domains\09_High_Performance_Log\README.md
 * Line: 236
 * Language: c
 * Block ID: 7c3c6df5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
