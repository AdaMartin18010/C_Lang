/*
 * Auto-generated from: 12_Practice_Exercises\08_Concurrency_Parallelism_Labs.md
 * Line: 15
 * Language: c
 * Block ID: c277dc43
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct ThreadPool ThreadPool;
typedef void (*TaskFunc)(void *arg);

// 创建线程池
ThreadPool* threadpool_create(size_t num_threads, size_t queue_capacity);

// 提交任务（阻塞直到队列有空间）
bool threadpool_submit(ThreadPool *pool, TaskFunc func, void *arg);

// 尝试提交（非阻塞）
bool threadpool_try_submit(ThreadPool *pool, TaskFunc func, void *arg);

// 优雅关闭（等待所有任务完成）
void threadpool_shutdown(ThreadPool *pool);

// 立即关闭（丢弃未执行任务）
void threadpool_shutdown_now(ThreadPool *pool);

// 等待所有任务完成
void threadpool_wait(ThreadPool *pool);

// 销毁线程池
void threadpool_destroy(ThreadPool *pool);

// 统计信息
size_t threadpool_active_count(const ThreadPool *pool);
size_t threadpool_pending_count(const ThreadPool *pool);
