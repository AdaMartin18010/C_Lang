# 并发与并行实验 (Concurrency & Parallelism Labs)

> **难度**: L4-L6 | **预计时间**: 30-50小时

---

## 实验1: 线程池实现

### 描述

实现生产者-消费者模型的通用线程池。

### 接口设计

```c
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
```

### 实现要点

- 使用条件变量实现任务队列的阻塞等待
- 支持动态扩容/缩容（可选）
- 优雅处理线程退出
- 避免惊群效应

### 测试用例

```c
// 计算斐波那契数列（递归任务）
void fib_task(void *arg);

// 并行MapReduce
void mapreduce_task(void *arg);

// 高并发压力测试
// 1000个线程提交100万个任务
```

---

## 实验2: 读写锁实现

### 描述

实现读写优先可配置的读写锁。

### 接口

```c
typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t read_cond;
    pthread_cond_t write_cond;
    int readers;
    int writers_waiting;
    bool writing;
    // 配置
    bool prefer_readers;  // true=读优先, false=写优先
} RWLock;

void rwlock_init(RWLock *lock, bool prefer_readers);
void rwlock_destroy(RWLock *lock);

void rwlock_read_lock(RWLock *lock);
bool rwlock_read_trylock(RWLock *lock);
void rwlock_read_unlock(RWLock *lock);

void rwlock_write_lock(RWLock *lock);
bool rwlock_write_trylock(RWLock *lock);
void rwlock_write_unlock(RWLock *lock);
```

### 特性

- 读优先：可能有写者饥饿
- 写优先：可能有读者饥饿
- 公平模式：先来先服务

---

## 实验3: 无锁数据结构

### 3.1 无锁队列 (Michael-Scott Queue)

```c
typedef struct Node {
    void *data;
    _Atomic(struct Node*) next;
} Node;

typedef struct {
    _Atomic(Node*) head;
    _Atomic(Node*) tail;
    atomic_size_t size;
} LockFreeQueue;

LockFreeQueue* lfqueue_create(void);
void lfqueue_destroy(LockFreeQueue *q);

bool lfqueue_enqueue(LockFreeQueue *q, void *data);
bool lfqueue_dequeue(LockFreeQueue *q, void **data);
size_t lfqueue_size(const LockFreeQueue *q);
```

### 3.2 无锁栈 (Treiber Stack)

```c
typedef struct {
    _Atomic(Node*) top;
    atomic_size_t size;
} LockFreeStack;

void lfstack_push(LockFreeStack *s, void *data);
bool lfstack_pop(LockFreeStack *s, void **data);
```

### 3.3 无锁哈希表

```c
typedef struct {
    _Atomic(Node*) *buckets;
    size_t bucket_count;
} LockFreeHashTable;

bool lfht_insert(LockFreeHashTable *ht, uint64_t key, void *value);
bool lfht_lookup(LockFreeHashTable *ht, uint64_t key, void **value);
bool lfht_remove(LockFreeHashTable *ht, uint64_t key);
```

### 实现要点

- 使用C11 `_Atomic` 和 `atomic_compare_exchange_weak/strong`
- 处理ABA问题（使用带版本号的指针或Hazard Pointers）
- 内存序选择（memory_order）

---

## 实验4: 并行排序

### 描述

实现多线程并行排序算法。

### 并行快速排序

```c
void parallel_quicksort(void *base, size_t nmemb, size_t size,
                        int (*cmp)(const void *, const void *),
                        size_t num_threads);
```

实现策略：

1. 主线程划分数据
2. 递归到子线程（当数据量>阈值时）
3. 任务窃取（工作队列）

### 并行归并排序

```c
void parallel_mergesort(void *base, size_t nmemb, size_t size,
                        int (*cmp)(const void *, const void *));
```

### 并行基数排序

```c
void parallel_radix_sort(uint64_t *arr, size_t n, size_t num_threads);
```

---

## 实验5: 生产者-消费者多缓冲区

### 描述

实现高性能双缓冲/多缓冲机制。

### 接口

```c
typedef struct DoubleBuffer DoubleBuffer;

// 创建双缓冲
DoubleBuffer* dbuf_create(size_t buffer_size);
void dbuf_destroy(DoubleBuffer *db);

// 生产者：获取写入缓冲区
void* dbuf_acquire_write(DoubleBuffer *db);
void dbuf_publish(DoubleBuffer *db);  // 切换缓冲区

// 消费者：获取读取缓冲区
const void* dbuf_acquire_read(DoubleBuffer *db);
void dbuf_release_read(DoubleBuffer *db);
```

### 应用场景

- 视频采集与处理
- 音频录制与播放
- 高频数据采集

---

## 实验6: 异步I/O与事件循环

### 描述

实现类似libuv的简单事件循环。

### 接口

```c
typedef struct EventLoop EventLoop;
typedef void (*IOCallback)(int fd, uint32_t events, void *userdata);
typedef void (*TimerCallback)(void *userdata);

EventLoop* evloop_create(void);
void evloop_destroy(EventLoop *loop);

// 注册I/O事件
void evloop_register(EventLoop *loop, int fd, uint32_t events,
                     IOCallback cb, void *userdata);
void evloop_unregister(EventLoop *loop, int fd);

// 定时器
void evloop_set_timeout(EventLoop *loop, uint64_t ms,
                        TimerCallback cb, void *userdata);

// 运行事件循环
void evloop_run(EventLoop *loop);
void evloop_stop(EventLoop *loop);
```

### 实现方式

- Linux: epoll
- macOS/BSD: kqueue
- Windows: IOCP（可选）

---

## 实验7: 内存序与原子操作实验

### 描述

深入理解C11内存序。

### 实验代码

```c
// 测试不同内存序的影响

typedef struct {
    atomic_int x;
    atomic_int y;
    atomic_int z;
} SharedData;

// 测试store/load的内存序
void test_memory_order(void);

// 实现seqlock（顺序锁）
typedef struct {
    atomic_ulong sequence;
    // 数据...
} SeqLock;

void seqlock_read_lock(SeqLock *sl);
bool seqlock_read_retry(SeqLock *sl, unsigned long start);
void seqlock_write_lock(SeqLock *sl);
void seqlock_write_unlock(SeqLock *sl);
```

---

## 实验8: OpenMP并行化

### 描述

使用OpenMP指令并行化算法。

### 练习

```c
// 并行矩阵乘法
void matmul_parallel(const double *a, const double *b,
                     double *c, size_t n);

// 并行归约（求和、求最大最小值）
double parallel_sum(const double *arr, size_t n);
double parallel_dot_product(const double *a, const double *b, size_t n);

// 并行for循环
void parallel_transform(double *arr, size_t n,
                        double (*func)(double));

// 任务并行
void parallel_tree_traversal(TreeNode *root);

// SIMD + OpenMP
void simd_parallel_add(float *a, const float *b, size_t n);
```

---

## 调试与验证

### 工具使用

```bash
# 检测数据竞争
valgrind --tool=helgrind ./your_program

# ThreadSanitizer
gcc -fsanitize=thread -g your_code.c -o your_program

# 性能分析
perf stat ./your_program
```

---

> **返回导航**: [练习模块总览](./README.md) | [知识库总览](../README.md)


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
