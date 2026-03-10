# 线程池模式 (Thread Pool Patterns)

## 目录

- [线程池模式 (Thread Pool Patterns)](#线程池模式-thread-pool-patterns)
  - [目录](#目录)
  - [基础线程池](#基础线程池)
    - [固定大小线程池](#固定大小线程池)
  - [工作窃取线程池](#工作窃取线程池)
    - [实现](#实现)
  - [Fork-Join框架](#fork-join框架)
    - [任务窃取Fork-Join](#任务窃取fork-join)
  - [任务调度策略](#任务调度策略)
    - [优先级调度](#优先级调度)
    - [工作分解](#工作分解)
  - [性能优化](#性能优化)
    - [避免伪共享](#避免伪共享)
    - [批量提交](#批量提交)
  - [参考](#参考)

---

## 基础线程池

### 固定大小线程池

```c
#include <threads.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>

typedef struct task {
    void (*fn)(void*);
    void* arg;
    struct task* next;
} task_t;

typedef struct {
    thrd_t* threads;
    int thread_count;

    /* 任务队列 */
    task_t* head;
    task_t* tail;
    mtx_t queue_lock;
    cnd_t task_available;
    cnd_t queue_empty;

    atomic_bool shutdown;
    atomic_int active_tasks;
    atomic_int pending_tasks;
} thread_pool_t;

/* 创建线程池 */
thread_pool_t* thread_pool_create(int num_threads) {
    thread_pool_t* pool = calloc(1, sizeof(thread_pool_t));
    pool->thread_count = num_threads;
    pool->threads = malloc(sizeof(thrd_t) * num_threads);

    mtx_init(&pool->queue_lock, mtx_plain);
    cnd_init(&pool->task_available);
    cnd_init(&pool->queue_empty);

    atomic_init(&pool->shutdown, false);
    atomic_init(&pool->active_tasks, 0);
    atomic_init(&pool->pending_tasks, 0);

    /* 创建工作线程 */
    for (int i = 0; i < num_threads; i++) {
        thrd_create(&pool->threads[i], worker_thread, pool);
    }

    return pool;
}

/* 提交任务 */
bool thread_pool_submit(thread_pool_t* pool, void (*fn)(void*), void* arg) {
    if (atomic_load(&pool->shutdown)) {
        return false;
    }

    task_t* task = malloc(sizeof(task_t));
    task->fn = fn;
    task->arg = arg;
    task->next = NULL;

    mtx_lock(&pool->queue_lock);

    if (pool->tail) {
        pool->tail->next = task;
    } else {
        pool->head = task;
    }
    pool->tail = task;

    atomic_fetch_add(&pool->pending_tasks, 1);
    cnd_signal(&pool->task_available);

    mtx_unlock(&pool->queue_lock);
    return true;
}

/* 工作线程 */
int worker_thread(void* arg) {
    thread_pool_t* pool = arg;

    while (1) {
        mtx_lock(&pool->queue_lock);

        /* 等待任务 */
        while (pool->head == NULL && !atomic_load(&pool->shutdown)) {
            cnd_wait(&pool->task_available, &pool->queue_lock);
        }

        if (atomic_load(&pool->shutdown) && pool->head == NULL) {
            mtx_unlock(&pool->queue_lock);
            break;
        }

        /* 获取任务 */
        task_t* task = pool->head;
        pool->head = task->next;
        if (pool->head == NULL) {
            pool->tail = NULL;
        }

        atomic_fetch_sub(&pool->pending_tasks, 1);
        atomic_fetch_add(&pool->active_tasks, 1);

        mtx_unlock(&pool->queue_lock);

        /* 执行任务 */
        task->fn(task->arg);
        free(task);

        atomic_fetch_sub(&pool->active_tasks, 1);

        /* 通知等待完成 */
        if (atomic_load(&pool->pending_tasks) == 0 &&
            atomic_load(&pool->active_tasks) == 0) {
            cnd_broadcast(&pool->queue_empty);
        }
    }

    return 0;
}

/* 优雅关闭 */
void thread_pool_shutdown(thread_pool_t* pool) {
    atomic_store(&pool->shutdown, true);
    cnd_broadcast(&pool->task_available);

    for (int i = 0; i < pool->thread_count; i++) {
        thrd_join(pool->threads[i], NULL);
    }
}

/* 等待所有任务完成 */
void thread_pool_wait(thread_pool_t* pool) {
    mtx_lock(&pool->queue_lock);
    while (atomic_load(&pool->pending_tasks) > 0 ||
           atomic_load(&pool->active_tasks) > 0) {
        cnd_wait(&pool->queue_empty, &pool->queue_lock);
    }
    mtx_unlock(&pool->queue_lock);
}
```

---

## 工作窃取线程池

### 实现

```c
#define QUEUE_SIZE 256
#define MAX_WORKERS 16

typedef struct {
    void* data;
    void (*fn)(void*);
} work_item_t;

typedef struct {
    work_item_t items[QUEUE_SIZE];
    _Atomic(int) head;  /* 窃取端 */
    _Atomic(int) tail;  /* 本地端 */
    char padding[64 - 2 * sizeof(_Atomic(int))];  /* 填充到缓存行 */
} work_queue_t;

typedef struct {
    int id;
    thrd_t thread;
    work_queue_t local_queue;
    struct thread_pool_ws* pool;
    atomic_bool running;
} worker_ws_t;

typedef struct thread_pool_ws {
    worker_ws_t workers[MAX_WORKERS];
    int worker_count;
    atomic_int next_worker;
} thread_pool_ws_t;

/* 本地入队 */
bool local_push(work_queue_t* q, work_item_t* item) {
    int tail = atomic_load_explicit(&q->tail, memory_order_relaxed);
    int next = (tail + 1) % QUEUE_SIZE;

    if (next == atomic_load_explicit(&q->head, memory_order_acquire)) {
        return false;  /* 满 */
    }

    q->items[tail] = *item;
    atomic_store_explicit(&q->tail, next, memory_order_release);
    return true;
}

/* 本地出队 */
bool local_pop(work_queue_t* q, work_item_t* item) {
    int tail = atomic_load_explicit(&q->tail, memory_order_relaxed) - 1;
    atomic_store_explicit(&q->tail, tail, memory_order_relaxed);
    atomic_thread_fence(memory_order_seq_cst);

    int head = atomic_load_explicit(&q->head, memory_order_relaxed);

    if (head <= tail) {
        *item = q->items[tail % QUEUE_SIZE];

        if (head != tail) {
            return true;
        }

        /* 最后一个元素 */
        if (!atomic_compare_exchange_strong_explicit(
            &q->head, &head, head + 1,
            memory_order_seq_cst, memory_order_relaxed)) {
            atomic_store_explicit(&q->tail, tail + 1, memory_order_relaxed);
            return false;
        }

        atomic_store_explicit(&q->tail, tail + 1, memory_order_relaxed);
        return true;
    }

    atomic_store_explicit(&q->tail, tail + 1, memory_order_relaxed);
    return false;
}

/* 窃取 */
bool steal(work_queue_t* victim, work_item_t* item) {
    int head = atomic_load_explicit(&victim->head, memory_order_acquire);
    atomic_thread_fence(memory_order_seq_cst);
    int tail = atomic_load_explicit(&victim->tail, memory_order_acquire);

    if (head < tail) {
        *item = victim->items[head % QUEUE_SIZE];

        if (atomic_compare_exchange_strong_explicit(
            &victim->head, &head, head + 1,
            memory_order_seq_cst, memory_order_relaxed)) {
            return true;
        }
    }
    return false;
}
```

---

## Fork-Join框架

### 任务窃取Fork-Join

```c
typedef struct fj_task {
    void (*compute)(struct fj_task*);
    void (*fork)(struct fj_task*);
    void* (*join)(struct fj_task*);

    atomic_int status;  /* 0=pending, 1=running, 2=done */
    struct fj_task* parent;
    struct fj_task* subtasks[2];
    atomic_int completed_subtasks;

    void* result;
} fj_task_t;

void fj_fork(fj_task_t* task) {
    atomic_store(&task->status, 1);
    /* 提交到线程池 */
    submit_to_pool(task);
}

void* fj_join(fj_task_t* task) {
    /* 如果任务未完成，尝试窃取其他工作 */
    while (atomic_load(&task->status) != 2) {
        work_item_t stolen;
        if (try_steal(&stolen)) {
            /* 执行窃取的任务 */
            stolen.fn(stolen.data);
        } else {
            thrd_yield();
        }
    }
    return task->result;
}

void fj_complete(fj_task_t* task) {
    atomic_store(&task->status, 2);

    if (task->parent) {
        int completed = atomic_fetch_add(&task->parent->completed_subtasks, 1);
        if (completed == 1) {  /* 两个子任务都完成 */
            /* 调度父任务 */
            fj_fork(task->parent);
        }
    }
}

/* 示例: 并行快速排序 */
void quicksort_fj(fj_task_t* task) {
    int* arr = task->data;
    int n = task->n;

    if (n < 1000) {
        /* 小数组直接排序 */
        qsort(arr, n, sizeof(int), compare_int);
        fj_complete(task);
        return;
    }

    /* 分区 */
    int pivot = partition(arr, n);

    /* 创建子任务 */
    fj_task_t* left = create_fj_task(quicksort_fj, arr, pivot);
    fj_task_t* right = create_fj_task(quicksort_fj, arr + pivot + 1, n - pivot - 1);

    left->parent = task;
    right->parent = task;
    task->subtasks[0] = left;
    task->subtasks[1] = right;

    /* Fork */
    fj_fork(left);
    fj_fork(right);
}
```

---

## 任务调度策略

### 优先级调度

```c
typedef enum {
    PRIORITY_LOW = 0,
    PRIORITY_NORMAL = 1,
    PRIORITY_HIGH = 2,
    PRIORITY_URGENT = 3,
    NUM_PRIORITIES
} priority_t;

typedef struct {
    task_t* queues[NUM_PRIORITIES];
    mtx_t locks[NUM_PRIORITIES];
    cnd_t conds[NUM_PRIORITIES];
} priority_pool_t;

/* 提交带优先级的任务 */
bool submit_priority(priority_pool_t* pool, priority_t prio,
                     void (*fn)(void*), void* arg) {
    task_t* task = create_task(fn, arg);

    mtx_lock(&pool->locks[prio]);
    enqueue(&pool->queues[prio], task);
    cnd_signal(&pool->conds[prio]);
    mtx_unlock(&pool->locks[prio]);
    return true;
}

/* 工作线程: 按优先级获取任务 */
task_t* get_next_task(priority_pool_t* pool) {
    /* 高优先级优先 */
    for (int prio = PRIORITY_URGENT; prio >= PRIORITY_LOW; prio--) {
        mtx_lock(&pool->locks[prio]);
        task_t* task = dequeue(&pool->queues[prio]);
        mtx_unlock(&pool->locks[prio]);

        if (task) return task;
    }
    return NULL;
}
```

### 工作分解

```c
/* 根据CPU负载动态调整 */
typedef struct {
    atomic_int load[MAX_WORKERS];  /* 每个工作线程的负载 */
} load_balancer_t;

int select_worker(load_balancer_t* lb, int num_workers) {
    /* 选择负载最低的工作者 */
    int min_load = INT_MAX;
    int selected = 0;

    for (int i = 0; i < num_workers; i++) {
        int load = atomic_load(&lb->load[i]);
        if (load < min_load) {
            min_load = load;
            selected = i;
        }
    }

    atomic_fetch_add(&lb->load[selected], 1);
    return selected;
}
```

---

## 性能优化

### 避免伪共享

```c
/* 错误: 多个线程写相邻变量 */
typedef struct {
    atomic_int counter1;  /* 线程1 */
    atomic_int counter2;  /* 线程2 */
} bad_counters_t;

/* 正确: 填充到不同缓存行 */
typedef struct {
    atomic_int counter;
    char padding[64 - sizeof(atomic_int)];
} padded_counter_t;

typedef struct {
    padded_counter_t counters[MAX_WORKERS];
} good_counters_t;
```

### 批量提交

```c
typedef struct {
    task_t* batch[64];
    int count;
    mtx_t lock;
} batch_queue_t;

void submit_batch(thread_pool_t* pool, batch_queue_t* bq,
                  void (*fn)(void*), void* arg) {
    task_t* task = create_task(fn, arg);

    mtx_lock(&bq->lock);
    bq->batch[bq->count++] = task;

    if (bq->count >= 64) {
        /* 批量提交 */
        mtx_lock(&pool->queue_lock);
        for (int i = 0; i < bq->count; i++) {
            enqueue(&pool->queue, bq->batch[i]);
        }
        mtx_unlock(&pool->queue_lock);
        cnd_broadcast(&pool->task_available);
        bq->count = 0;
    }
    mtx_unlock(&bq->lock);
}
```

---

## 参考

- [Java Fork/Join Framework](https://docs.oracle.com/javase/tutorial/essential/concurrency/forkjoin.html)
- [Intel TBB](https://github.com/oneapi-src/oneTBB)
