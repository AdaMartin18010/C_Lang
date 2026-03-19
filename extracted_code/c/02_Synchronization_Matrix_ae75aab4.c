/*
 * Auto-generated from: 06_Thinking_Representation\02_Comparison_Matrices\02_Synchronization_Matrix.md
 * Line: 1284
 * Language: c
 * Block ID: ae75aab4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// complete_sync_example.c
#include <pthread.h>
#include <semaphore.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// 线程安全的数据结构 - 综合示例

typedef struct {
    // 使用读写锁保护数据
    pthread_rwlock_t rwlock;

    // 使用互斥锁保护计数器
    pthread_mutex_t count_mutex;
    int read_count;
    int write_count;

    // 使用原子操作保护标志
    atomic_bool valid;

    // 实际数据
    char *data;
    size_t size;
} thread_safe_buffer_t;

// 初始化
int ts_buffer_init(thread_safe_buffer_t *buf, size_t initial_size) {
    buf->data = malloc(initial_size);
    if (!buf->data) return -1;

    buf->size = initial_size;
    buf->read_count = 0;
    buf->write_count = 0;

    pthread_rwlock_init(&buf->rwlock, NULL);
    pthread_mutex_init(&buf->count_mutex, NULL);
    atomic_init(&buf->valid, true);

    return 0;
}

// 清理
void ts_buffer_destroy(thread_safe_buffer_t *buf) {
    atomic_store(&buf->valid, false);
    pthread_rwlock_wrlock(&buf->rwlock);

    free(buf->data);
    buf->data = NULL;
    buf->size = 0;

    pthread_rwlock_unlock(&buf->rwlock);
    pthread_rwlock_destroy(&buf->rwlock);
    pthread_mutex_destroy(&buf->count_mutex);
}

// 读操作
size_t ts_buffer_read(thread_safe_buffer_t *buf, char *out, size_t offset, size_t len) {
    // 检查有效性
    if (!atomic_load(&buf->valid)) return 0;

    // 获取读锁
    pthread_rwlock_rdlock(&buf->rwlock);

    // 更新读计数
    pthread_mutex_lock(&buf->count_mutex);
    buf->read_count++;
    pthread_mutex_unlock(&buf->count_mutex);

    // 执行读取
    size_t to_read = (offset + len <= buf->size) ? len : (buf->size - offset);
    if (out && to_read > 0) {
        memcpy(out, buf->data + offset, to_read);
    }

    pthread_rwlock_unlock(&buf->rwlock);
    return to_read;
}

// 写操作
int ts_buffer_write(thread_safe_buffer_t *buf, const char *in, size_t offset, size_t len) {
    if (!atomic_load(&buf->valid)) return -1;

    pthread_rwlock_wrlock(&buf->rwlock);

    pthread_mutex_lock(&buf->count_mutex);
    buf->write_count++;
    pthread_mutex_unlock(&buf->count_mutex);

    // 检查是否需要扩容
    if (offset + len > buf->size) {
        size_t new_size = offset + len;
        char *new_data = realloc(buf->data, new_size);
        if (!new_data) {
            pthread_rwlock_unlock(&buf->rwlock);
            return -1;
        }
        buf->data = new_data;
        buf->size = new_size;
    }

    memcpy(buf->data + offset, in, len);

    pthread_rwlock_unlock(&buf->rwlock);
    return 0;
}

// 测试代码
#define NUM_READERS 5
#define NUM_WRITERS 2
#define OPERATIONS 1000

void *reader(void *arg) {
    thread_safe_buffer_t *buf = arg;
    char read_buf[100];

    for (int i = 0; i < OPERATIONS; i++) {
        size_t n = ts_buffer_read(buf, read_buf, 0, sizeof(read_buf));
        (void)n;
        usleep(rand() % 1000);
    }
    return NULL;
}

void *writer(void *arg) {
    thread_safe_buffer_t *buf = arg;
    const char *msg = "Hello, World!";

    for (int i = 0; i < OPERATIONS; i++) {
        ts_buffer_write(buf, msg, 0, strlen(msg));
        usleep(rand() % 2000);
    }
    return NULL;
}

int main(void) {
    thread_safe_buffer_t buffer;
    pthread_t readers[NUM_READERS];
    pthread_t writers[NUM_WRITERS];

    printf("=== 综合同步示例 ===\n");

    if (ts_buffer_init(&buffer, 1024) != 0) {
        perror("Failed to initialize buffer");
        return 1;
    }

    // 创建读者和写者线程
    for (int i = 0; i < NUM_READERS; i++) {
        pthread_create(&readers[i], NULL, reader, &buffer);
    }
    for (int i = 0; i < NUM_WRITERS; i++) {
        pthread_create(&writers[i], NULL, writer, &buffer);
    }

    // 等待完成
    for (int i = 0; i < NUM_READERS; i++) {
        pthread_join(readers[i], NULL);
    }
    for (int i = 0; i < NUM_WRITERS; i++) {
        pthread_join(writers[i], NULL);
    }

    printf("Read operations: %d\n", buffer.read_count);
    printf("Write operations: %d\n", buffer.write_count);

    ts_buffer_destroy(&buffer);

    printf("\nAll operations completed successfully!\n");
    return 0;
}
