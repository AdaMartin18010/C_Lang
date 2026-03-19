/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\06_Standard_Library_Physics\02_Memcpy_SIMD.md
 * Line: 530
 * Language: c
 * Block ID: a184846d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 超大块内存拷贝的多线程并行化：
 *
 * - 主线程协调
 * - 工作线程并行处理块
 * - 需要同步点
 */

#include <pthread.h>

typedef struct {
    void *dst;
    const void *src;
    size_t offset;
    size_t size;
} CopyTask;

void* thread_memcpy_worker(void *arg) {
    CopyTask *task = arg;
    memcpy_optimized((char*)task->dst + task->offset,
                    (const char*)task->src + task->offset,
                    task->size);
    return NULL;
}

void* memcpy_parallel(void *dst, const void *src, size_t n, int num_threads) {
    if (n < 1024 * 1024 || num_threads <= 1) {
        // 小块或单线程：直接拷贝
        return memcpy_optimized(dst, src, n);
    }

    pthread_t threads[num_threads];
    CopyTask tasks[num_threads];

    size_t chunk_size = n / num_threads;
    chunk_size = (chunk_size + 63) & ~63;  // 64字节对齐

    // 创建工作线程
    for (int i = 0; i < num_threads; i++) {
        tasks[i] = (CopyTask){
            .dst = dst,
            .src = src,
            .offset = i * chunk_size,
            .size = (i == num_threads - 1) ?
                    (n - i * chunk_size) : chunk_size
        };

        pthread_create(&threads[i], NULL,
                      thread_memcpy_worker, &tasks[i]);
    }

    // 等待完成
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    return dst;
}
