/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\10_Threads_C11.md
 * Line: 1419
 * Language: c
 * Block ID: 154bf50d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <stdbool.h>
#include <string.h>

/* ========== 线程池核心数据结构 ========== */

/* 任务结构 */
typedef struct Task {
    void (*function)(void*);
    void* argument;
    struct Task* next;
} Task;

/* 线程池配置 */
typedef struct {
    size_t thread_count;       /* 工作线程数 */
    size_t queue_capacity;     /* 任务队列容量 */
    int shutdown_timeout_ms;   /* 关闭超时 */
} ThreadPoolConfig;

/* 线程池结构 */
typedef struct {
    /* 线程数组 */
    thrd_t* threads;
    size_t thread_count;

    /* 任务队列 */
    Task* task_head;
    Task* task_tail;
    size_t task_count;
    size_t queue_capacity;

    /* 同步原语 */
    mtx_t mutex;
    cnd_t task_available;   /* 有新任务 */
    cnd_t queue_not_full;   /* 队列有空位 */
    cnd_t all_idle;         /* 所有线程空闲 */

    /* 状态 */
    bool shutdown;
    bool immediate_shutdown;
    size_t active_count;    /* 正在执行任务的线程数 */
    size_t idle_count;      /* 空闲线程数 */

    /* 统计 */
    atomic_size_t tasks_submitted;
    atomic_size_t tasks_completed;
    atomic_size_t tasks_rejected;
} ThreadPool;

/* 线程参数 */
typedef struct {
    ThreadPool* pool;
    size_t id;
} WorkerArg;

/* ========== 线程池实现 ========== */

/* 工作线程 */
int worker_thread(void* arg) {
    WorkerArg* worker_arg = (WorkerArg*)arg;
    ThreadPool* pool = worker_arg->pool;
    size_t id = worker_arg->id;
    free(worker_arg);

    printf("[Worker %zu] Started\n", id);

    while (true) {
        Task* task = NULL;

        mtx_lock(&pool->mutex);

        /* 等待任务或关闭信号 */
        while (pool->task_head == NULL && !pool->shutdown) {
            pool->idle_count++;
            if (pool->idle_count == pool->thread_count) {
                cnd_signal(&pool->all_idle);
            }
            cnd_wait(&pool->task_available, &pool->mutex);
            pool->idle_count--;
        }

        /* 检查关闭状态 */
        if (pool->shutdown) {
            if (pool->immediate_shutdown || pool->task_head == NULL) {
                mtx_unlock(&pool->mutex);
                break;
            }
        }

        /* 获取任务 */
        if (pool->task_head != NULL) {
            task = pool->task_head;
            pool->task_head = task->next;
            if (pool->task_head == NULL) {
                pool->task_tail = NULL;
            }
            pool->task_count--;
            pool->active_count++;
            cnd_signal(&pool->queue_not_full);
        }

        mtx_unlock(&pool->mutex);

        /* 执行任务 */
        if (task) {
            task->function(task->argument);
            free(task);

            mtx_lock(&pool->mutex);
            pool->active_count--;
            atomic_fetch_add_explicit(&pool->tasks_completed, 1,
                                       memory_order_relaxed);
            mtx_unlock(&pool->mutex);
        }
    }

    printf("[Worker %zu] Exiting\n", id);
    return 0;
}

/* 创建线程池 */
ThreadPool* threadpool_create(const ThreadPoolConfig* config) {
    ThreadPool* pool = calloc(1, sizeof(ThreadPool));
    if (!pool) return NULL;

    pool->thread_count = config->thread_count;
    pool->queue_capacity = config->queue_capacity;

    /* 初始化同步原语 */
    mtx_init(&pool->mutex, mtx_plain);
    cnd_init(&pool->task_available);
    cnd_init(&pool->queue_not_full);
    cnd_init(&pool->all_idle);

    /* 创建工作线程 */
    pool->threads = calloc(pool->thread_count, sizeof(thrd_t));
    if (!pool->threads) {
        free(pool);
        return NULL;
    }

    for (size_t i = 0; i < pool->thread_count; i++) {
        WorkerArg* arg = malloc(sizeof(WorkerArg));
        arg->pool = pool;
        arg->id = i;

        if (thrd_create(&pool->threads[i], worker_thread, arg)
            != thrd_success) {
            free(arg);
            /* 清理已创建的线程 */
            pool->shutdown = true;
            cnd_broadcast(&pool->task_available);
            for (size_t j = 0; j < i; j++) {
                thrd_join(pool->threads[j], NULL);
            }
            free(pool->threads);
            free(pool);
            return NULL;
        }
    }

    printf("[ThreadPool] Created with %zu workers, queue capacity %zu\n",
           pool->thread_count, pool->queue_capacity);
    return pool;
}

/* 提交任务 */
bool threadpool_submit(ThreadPool* pool, void (*func)(void*), void* arg,
                       int timeout_ms) {
    if (!pool || !func || pool->shutdown) {
        return false;
    }

    Task* task = malloc(sizeof(Task));
    if (!task) return false;

    task->function = func;
    task->argument = arg;
    task->next = NULL;

    mtx_lock(&pool->mutex);

    /* 等待队列有空位 */
    while (pool->task_count >= pool->queue_capacity && !pool->shutdown) {
        if (timeout_ms == 0) {
            mtx_unlock(&pool->mutex);
            free(task);
            atomic_fetch_add(&pool->tasks_rejected, 1);
            return false;
        } else if (timeout_ms < 0) {
            cnd_wait(&pool->queue_not_full, &pool->mutex);
        } else {
            struct timespec deadline;
            timespec_get(&deadline, TIME_UTC);
            deadline.tv_nsec += timeout_ms * 1000000;
            if (cnd_timedwait(&pool->queue_not_full, &pool->mutex,
                              &deadline) == thrd_timeout) {
                mtx_unlock(&pool->mutex);
                free(task);
                atomic_fetch_add(&pool->tasks_rejected, 1);
                return false;
            }
        }
    }

    if (pool->shutdown) {
        mtx_unlock(&pool->mutex);
        free(task);
        return false;
    }

    /* 添加到队列尾部 */
    if (pool->task_tail) {
        pool->task_tail->next = task;
    } else {
        pool->task_head = task;
    }
    pool->task_tail = task;
    pool->task_count++;
    atomic_fetch_add(&pool->tasks_submitted, 1);

    cnd_signal(&pool->task_available);
    mtx_unlock(&pool->mutex);

    return true;
}

/* 等待所有任务完成 */
bool threadpool_wait_idle(ThreadPool* pool, int timeout_ms) {
    if (!pool) return false;

    mtx_lock(&pool->mutex);

    while (pool->active_count > 0 || pool->task_count > 0) {
        if (timeout_ms < 0) {
            cnd_wait(&pool->all_idle, &pool->mutex);
        } else {
            struct timespec deadline;
            timespec_get(&deadline, TIME_UTC);
            deadline.tv_nsec += timeout_ms * 1000000;
            if (cnd_timedwait(&pool->all_idle, &pool->mutex, &deadline)
                == thrd_timeout) {
                mtx_unlock(&pool->mutex);
                return false;
            }
        }
    }

    mtx_unlock(&pool->mutex);
    return true;
}

/* 销毁线程池 */
void threadpool_destroy(ThreadPool* pool, bool immediate) {
    if (!pool) return;

    mtx_lock(&pool->mutex);
    pool->shutdown = true;
    pool->immediate_shutdown = immediate;
    mtx_unlock(&pool->mutex);

    /* 唤醒所有线程 */
    cnd_broadcast(&pool->task_available);

    /* 等待所有线程退出 */
    for (size_t i = 0; i < pool->thread_count; i++) {
        thrd_join(pool->threads[i], NULL);
    }

    /* 清理未执行的任务 */
    if (!immediate) {
        Task* task = pool->task_head;
        while (task) {
            Task* next = task->next;
            free(task);
            task = next;
        }
    }

    printf("[ThreadPool] Destroyed. Submitted: %zu, Completed: %zu, "
           "Rejected: %zu\n",
           atomic_load(&pool->tasks_submitted),
           atomic_load(&pool->tasks_completed),
           atomic_load(&pool->tasks_rejected));

    free(pool->threads);
    mtx_destroy(&pool->mutex);
    cnd_destroy(&pool->task_available);
    cnd_destroy(&pool->queue_not_full);
    cnd_destroy(&pool->all_idle);
    free(pool);
}

/* ========== 使用示例 ========== */

static atomic_int g_task_counter = 0;

void sample_task(void* arg) {
    int task_id = *(int*)arg;
    free(arg);

    printf("[Task %d] Executing on thread\n", task_id);

    /* 模拟工作 */
    thrd_sleep(&(struct timespec){.tv_nsec = 50000000}, NULL);

    atomic_fetch_add(&g_task_counter, 1);
    printf("[Task %d] Completed\n", task_id);
}

int main(void) {
    printf("=== Thread Pool Demo ===\n\n");

    ThreadPoolConfig config = {
        .thread_count = 4,
        .queue_capacity = 10,
        .shutdown_timeout_ms = 5000
    };

    ThreadPool* pool = threadpool_create(&config);
    if (!pool) {
        fprintf(stderr, "Failed to create thread pool\n");
        return 1;
    }

    /* 提交20个任务 */
    printf("Submitting 20 tasks...\n");
    for (int i = 0; i < 20; i++) {
        int* id = malloc(sizeof(int));
        *id = i;

        if (!threadpool_submit(pool, sample_task, id, 1000)) {
            printf("Failed to submit task %d\n", i);
            free(id);
        }
    }

    /* 等待所有任务完成 */
    printf("Waiting for all tasks to complete...\n");
    threadpool_wait_idle(pool, -1);

    printf("\nTotal tasks executed: %d\n", atomic_load(&g_task_counter));

    /* 优雅关闭 */
    threadpool_destroy(pool, false);

    printf("\n=== Thread Pool Demo Complete ===\n");
    return 0;
}
