# C11 并发编程完全指南

> **难度**: L3-L5
> **对应标准**: ISO C11 §7.26 (threads.h), §7.17 (stdatomic.h)
> **核心目标**: 掌握标准C的跨平台并发编程

---

## 1. C11 线程模型

### 1.1 基本线程操作

```c
#include <threads.h>
#include <stdio.h>

// 线程入口函数
int worker_thread(void *arg) {
    int id = *(int*)arg;
    printf("Thread %d running\n", id);
    return id * 10;  // 返回值可通过 thrd_join 获取
}

int main(void) {
    thrd_t t1, t2;
    int id1 = 1, id2 = 2;

    // 创建线程
    thrd_create(&t1, worker_thread, &id1);
    thrd_create(&t2, worker_thread, &id2);

    // 等待线程完成
    int result1, result2;
    thrd_join(t1, &result1);
    thrd_join(t2, &result2);

    printf("Results: %d, %d\n", result1, result2);
    return 0;
}
```

### 1.2 线程局部存储

```c
_Thread_local int thread_local_counter = 0;

// 每个线程有自己独立的副本
void increment(void) {
    thread_local_counter++;
}
```

---

## 2. 互斥锁与同步

### 2.1 基本互斥锁

```c
#include <threads.h>

typedef struct {
    mtx_t lock;
    int count;
} Counter;

void counter_init(Counter *c) {
    mtx_init(&c->lock, mtx_plain);
    c->count = 0;
}

void counter_increment(Counter *c) {
    mtx_lock(&c->lock);
    c->count++;
    mtx_unlock(&c->lock);
}

void counter_destroy(Counter *c) {
    mtx_destroy(&c->lock);
}
```

### 2.2 条件变量

```c
typedef struct {
    mtx_t lock;
    cnd_t cond;
    int ready;
} Signal;

void signal_wait(Signal *s) {
    mtx_lock(&s->lock);
    while (!s->ready) {          // 必须用 while，防止虚假唤醒
        cnd_wait(&s->cond, &s->lock);
    }
    // 条件满足，执行工作
    mtx_unlock(&s->lock);
}

void signal_notify(Signal *s) {
    mtx_lock(&s->lock);
    s->ready = 1;
    cnd_broadcast(&s->cond);     // 唤醒所有等待线程
    mtx_unlock(&s->lock);
}
```

### 2.3 读写锁模拟（C11无原生rwlock）

```c
typedef struct {
    mtx_t lock;
    cnd_t readers_done;
    int reader_count;
    int writer_waiting;
} RWLock;

void rwlock_read_lock(RWLock *rw) {
    mtx_lock(&rw->lock);
    while (rw->writer_waiting) {
        cnd_wait(&rw->readers_done, &rw->lock);
    }
    rw->reader_count++;
    mtx_unlock(&rw->lock);
}

void rwlock_read_unlock(RWLock *rw) {
    mtx_lock(&rw->lock);
    rw->reader_count--;
    if (rw->reader_count == 0) {
        cnd_broadcast(&rw->readers_done);
    }
    mtx_unlock(&rw->lock);
}
```

---

## 3. 原子操作

### 3.1 原子类型与操作

```c
#include <stdatomic.h>

_Atomic int counter = 0;

// 原子递增
atomic_fetch_add(&counter, 1);   // 返回旧值

// 原子比较并交换（CAS）
int expected = 0;
atomic_compare_exchange_strong(&counter, &expected, 1);
// 如果 counter == expected(0)，则设为1，返回true
// 否则将expected更新为counter的当前值，返回false
```

### 3.2 内存序（Memory Order）

```c
_Atomic int flag = 0;
int data = 0;

// Thread 1
void producer(void) {
    data = 42;
    atomic_store_explicit(&flag, 1, memory_order_release);
    // release: 保证此操作之前的写操作对其他线程可见
}

// Thread 2
void consumer(void) {
    while (atomic_load_explicit(&flag, memory_order_acquire) != 1) {
        // 自旋等待
    }
    // acquire: 保证能看到 release 线程之前的所有写操作
    printf("data = %d\n", data);  // 保证输出 42
}
```

| 内存序 | 保证 | 开销 |
|:-------|:-----|:-----|
| `memory_order_relaxed` | 无顺序保证 | 最低 |
| `memory_order_acquire` | 读操作，看到之前的 release | 中 |
| `memory_order_release` | 写操作，对后续 acquire 可见 | 中 |
| `memory_order_acq_rel` | acquire + release | 中 |
| `memory_order_seq_cst` | 全序（默认，最强） | 最高 |

### 3.3 无锁计数器

```c
typedef struct {
    _Atomic int count;
} LockFreeCounter;

void lf_counter_init(LockFreeCounter *c) {
    atomic_init(&c->count, 0);
}

void lf_counter_increment(LockFreeCounter *c) {
    atomic_fetch_add_explicit(&c->count, 1, memory_order_relaxed);
}

int lf_counter_get(LockFreeCounter *c) {
    return atomic_load_explicit(&c->count, memory_order_acquire);
}
```

---

## 4. 常见并发模式

### 4.1 线程池

```c
#define MAX_TASKS 64
#define MAX_THREADS 4

typedef struct {
    void (*func)(void*);
    void *arg;
} Task;

typedef struct {
    Task queue[MAX_TASKS];
    int head, tail, count;
    mtx_t lock;
    cnd_t not_empty;
    cnd_t not_full;
    int shutdown;
} ThreadPool;

void pool_submit(ThreadPool *pool, void (*func)(void*), void *arg) {
    mtx_lock(&pool->lock);
    while (pool->count == MAX_TASKS && !pool->shutdown) {
        cnd_wait(&pool->not_full, &pool->lock);
    }
    if (pool->shutdown) {
        mtx_unlock(&pool->lock);
        return;
    }
    pool->queue[pool->tail].func = func;
    pool->queue[pool->tail].arg = arg;
    pool->tail = (pool->tail + 1) % MAX_TASKS;
    pool->count++;
    cnd_signal(&pool->not_empty);
    mtx_unlock(&pool->lock);
}

int pool_worker(void *arg) {
    ThreadPool *pool = arg;
    while (1) {
        mtx_lock(&pool->lock);
        while (pool->count == 0 && !pool->shutdown) {
            cnd_wait(&pool->not_empty, &pool->lock);
        }
        if (pool->shutdown) {
            mtx_unlock(&pool->lock);
            break;
        }
        Task task = pool->queue[pool->head];
        pool->head = (pool->head + 1) % MAX_TASKS;
        pool->count--;
        cnd_signal(&pool->not_full);
        mtx_unlock(&pool->lock);

        task.func(task.arg);  // 执行任务
    }
    return 0;
}
```

### 4.2 单例模式（线程安全）

```c
static _Atomic(Instance*) instance = NULL;
static mtx_t instance_lock;

Instance *get_instance(void) {
    Instance *inst = atomic_load_explicit(&instance, memory_order_acquire);
    if (inst == NULL) {
        mtx_lock(&instance_lock);
        inst = atomic_load_explicit(&instance, memory_order_relaxed);
        if (inst == NULL) {
            inst = create_instance();
            atomic_store_explicit(&instance, inst, memory_order_release);
        }
        mtx_unlock(&instance_lock);
    }
    return inst;
}
```

---

## 5. 与 POSIX pthread 的对比

| 特性 | C11 threads.h | POSIX pthread |
|:-----|:--------------|:--------------|
| 可移植性 | 标准C（但支持有限） | POSIX系统 |
| MSVC | ❌ 不支持 | ❌ 不支持（用Win32） |
| 线程创建 | `thrd_create` | `pthread_create` |
| 互斥锁 | `mtx_t` | `pthread_mutex_t` |
| 条件变量 | `cnd_t` | `pthread_cond_t` |
| 线程局部 | `_Thread_local` | `__thread` / `pthread_key` |
| 原子操作 | `stdatomic.h` | 无（需GCC内置） |

> **实际建议**: Linux/macOS 使用 pthread；需要原子操作时用 C11 `stdatomic.h`；Windows 使用原生线程API或跨平台库（如uv_thread）。

---

## 6. 检查清单

- [ ] 共享数据访问始终使用互斥锁或原子操作
- [ ] 条件变量等待使用 `while` 而非 `if`
- [ ] 锁的获取和释放成对出现（考虑异常路径）
- [ ] 避免在持有锁时调用可能阻塞的函数
- [ ] 使用 `memory_order_seq_cst` 除非已理解更弱的内存序

---

## 7. 配套代码示例

本节配套可编译代码位于 `examples/concurrency/` 目录：

| 示例文件 | 演示内容 | 编译命令 |
|---------|---------|---------|
| `c11_threads.c` | `thrd_create`/`thrd_join` + `mtx_t` 互斥锁 | `gcc -O2 -std=c11` |
| `producer_consumer.c` | `cnd_t` 条件变量实现有界缓冲区 | `gcc -O2 -std=c11` |
| `atomic_operations.c` | `atomic_int` / `atomic_flag` 自旋锁 | `gcc -O2 -std=c11` |
| `pthread_compat.c` | POSIX pthread 兼容版本（MinGW/无 C11 threads 时使用） | `gcc -O2 -lpthread` |

### 同步原语性能对比（x86-64）

| 原语 | 无竞争延迟 | 竞争时行为 | 适用场景 |
|------|----------|-----------|---------|
| `mtx_t` (mutex) | ~25 ns | 线程休眠 | 临界区 > 1μs |
| `atomic_flag` (spinlock) | ~10 ns | 忙等待 | 临界区 < 1μs |
| `atomic_*` (seq_cst) | ~15 ns | 硬件缓存一致性 | 计数器、标志位 |
| `atomic_*` (relaxed) | ~5 ns | 无全局顺序 | 独立计数器 |

---

## 8. 常见错误模式

| 错误模式 | 后果 | 修复方案 |
|---------|------|---------|
| **条件变量用 `if` 而非 `while`** | 虚假唤醒导致断言失败 | 始终 `while (!condition) cnd_wait(...)` |
| **锁内调用可能阻塞的函数** | 死锁，性能暴跌 | 临界区内只做最少工作 |
| **忘记解锁（异常路径）** | 死锁 | 使用 `goto cleanup` 确保成对释放 |
| **数据竞争（非原子访问）** | 未定义行为，结果不可复现 | 所有共享变量用 `atomic` 或 mutex 保护 |
| **错误的内存序** | 指令重排导致可见性问题 | 默认使用 `memory_order_seq_cst` |
| **ABA 问题（无锁结构）** | 指针复用导致状态不一致 | 使用带标签指针或 hazard pointer |

---

> **最后更新**: 2026-05-11
> **参考**: ISO C11 §7.26, §7.17, C++ Concurrency in Action (Williams)
