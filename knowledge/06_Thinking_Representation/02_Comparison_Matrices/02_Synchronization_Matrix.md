# 同步原语对比矩阵


---

## 📑 目录

- [同步原语对比矩阵](#同步原语对比矩阵)
  - [📑 目录](#-目录)
  - [概述](#概述)
  - [互斥锁、读写锁、自旋锁、信号量、条件变量](#互斥锁读写锁自旋锁信号量条件变量)
    - [POSIX线程同步原语对比](#posix线程同步原语对比)
    - [互斥锁 (Mutex) 详解](#互斥锁-mutex-详解)
    - [读写锁 (RWLock) 详解](#读写锁-rwlock-详解)
    - [自旋锁 (Spinlock) 详解](#自旋锁-spinlock-详解)
    - [信号量 (Semaphore) 详解](#信号量-semaphore-详解)
    - [条件变量 (Condition Variable) 详解](#条件变量-condition-variable-详解)
  - [C11原子操作](#c11原子操作)
    - [C11原子类型和操作](#c11原子类型和操作)
  - [性能/适用场景/复杂度对比](#性能适用场景复杂度对比)
    - [对比矩阵: 各种同步机制的特性](#对比矩阵-各种同步机制的特性)
    - [性能对比表](#性能对比表)
  - [死锁避免策略](#死锁避免策略)
  - [对比矩阵: 各种同步机制的特性](#对比矩阵-各种同步机制的特性-1)
  - [C代码示例: 各种锁的使用](#c代码示例-各种锁的使用)
  - [编译和运行](#编译和运行)


---

## 概述

多线程编程中的同步机制是确保数据一致性和程序正确性的关键。
不同的同步原语有不同的性能特征、适用场景和使用复杂度。
本文详细对比各种C语言同步机制，并提供实战代码示例。

## 互斥锁、读写锁、自旋锁、信号量、条件变量

### POSIX线程同步原语对比

```c
// sync_primitives_overview.c
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * 同步原语概览:
 *
 * 1. 互斥锁 (Mutex)
 *    - 用途: 互斥访问临界区
 *    - 特性: 阻塞等待，上下文切换开销
 *    - 适用: 临界区执行时间较长
 *
 * 2. 读写锁 (RWLock)
 *    - 用途: 读多写少的场景
 *    - 特性: 读共享，写独占
 *    - 适用: 读操作远多于写操作
 *
 * 3. 自旋锁 (Spinlock)
 *    - 用途: 短暂临界区保护
 *    - 特性: 忙等待，无上下文切换
 *    - 适用: 多核CPU，临界区极短
 *
 * 4. 信号量 (Semaphore)
 *    - 用途: 资源计数和访问控制
 *    - 特性: 可多个线程同时访问
 *    - 适用: 连接池、资源池等
 *
 * 5. 条件变量 (Condition Variable)
 *    - 用途: 线程间事件通知
 *    - 特性: 必须与互斥锁配合使用
 *    - 适用: 生产者-消费者模式
 */

int main(void) {
    printf("POSIX线程同步原语:\n");
    printf("- pthread_mutex_t: 互斥锁\n");
    printf("- pthread_rwlock_t: 读写锁\n");
    printf("- pthread_spinlock_t: 自旋锁\n");
    printf("- sem_t: 信号量\n");
    printf("- pthread_cond_t: 条件变量\n");
    return 0;
}
```

### 互斥锁 (Mutex) 详解

```c
// mutex_demo.c
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 4
#define INCREMENTS 1000000

typedef struct {
    pthread_mutex_t mutex;
    long counter;
    long read_counter;
} shared_data_t;

shared_data_t data = {
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .counter = 0,
    .read_counter = 0
};

// 普通互斥锁
void *mutex_worker(void *arg) {
    (void)arg;
    for (int i = 0; i < INCREMENTS; i++) {
        pthread_mutex_lock(&data.mutex);
        data.counter++;
        pthread_mutex_unlock(&data.mutex);
    }
    return NULL;
}

// 尝试加锁 (非阻塞)
void *trylock_worker(void *arg) {
    (void)arg;
    int locked = 0;
    for (int i = 0; i < INCREMENTS; i++) {
        // 尝试获取锁，不阻塞
        if (pthread_mutex_trylock(&data.mutex) == 0) {
            data.counter++;
            pthread_mutex_unlock(&data.mutex);
            locked++;
        } else {
            // 获取失败，做其他工作
            __asm__ volatile ("nop");  // 忙等待替代工作
        }
    }
    printf("Thread locked %d times\n", locked);
    return NULL;
}

// 定时加锁
void *timedlock_worker(void *arg) {
    (void)arg;
    struct timespec timeout;
    int success = 0, timeout_count = 0;

    for (int i = 0; i < 100; i++) {
        clock_gettime(CLOCK_REALTIME, &timeout);
        timeout.tv_nsec += 1000000;  // 1ms

        if (pthread_mutex_timedlock(&data.mutex, &timeout) == 0) {
            data.counter++;
            pthread_mutex_unlock(&data.mutex);
            success++;
        } else {
            timeout_count++;
        }
    }
    printf("Success: %d, Timeouts: %d\n", success, timeout_count);
    return NULL;
}

// 递归互斥锁示例
void recursive_function(shared_data_t *d, int depth);

pthread_mutex_t recursive_mutex;
pthread_mutexattr_t recursive_attr;

void *recursive_worker(void *arg) {
    (void)arg;

    // 初始化递归互斥锁
    pthread_mutexattr_init(&recursive_attr);
    pthread_mutexattr_settype(&recursive_attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&recursive_mutex, &recursive_attr);

    recursive_function(&data, 5);

    pthread_mutex_destroy(&recursive_mutex);
    pthread_mutexattr_destroy(&recursive_attr);
    return NULL;
}

void recursive_function(shared_data_t *d, int depth) {
    if (depth <= 0) return;

    pthread_mutex_lock(&recursive_mutex);  // 第一次加锁
    d->read_counter++;

    recursive_function(d, depth - 1);  // 递归，再次加锁

    pthread_mutex_unlock(&recursive_mutex);
}

int main(void) {
    pthread_t threads[NUM_THREADS];

    printf("=== 互斥锁演示 ===\n");

    // 普通互斥锁测试
    printf("\n1. 普通互斥锁测试\n");
    data.counter = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, mutex_worker, NULL);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    printf("Final counter: %ld (expected: %d)\n",
           data.counter, NUM_THREADS * INCREMENTS);

    // 尝试加锁测试
    printf("\n2. 尝试加锁测试\n");
    data.counter = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, trylock_worker, NULL);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    printf("Final counter: %ld\n", data.counter);

    // 递归互斥锁测试
    printf("\n3. 递归互斥锁测试\n");
    data.read_counter = 0;
    pthread_t recursive_thread;
    pthread_create(&recursive_thread, NULL, recursive_worker, NULL);
    pthread_join(recursive_thread, NULL);
    printf("Recursive counter: %ld\n", data.read_counter);

    pthread_mutex_destroy(&data.mutex);
    return 0;
}
```

### 读写锁 (RWLock) 详解

```c
// rwlock_demo.c
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_READERS 8
#define NUM_WRITERS 2
#define OPERATIONS 100000

typedef struct {
    pthread_rwlock_t rwlock;
    int data;
    int read_count;
    int write_count;
} database_t;

database_t db = {
    .rwlock = PTHREAD_RWLOCK_INITIALIZER,
    .data = 0,
    .read_count = 0,
    .write_count = 0
};

// 读者线程
void *reader_thread(void *arg) {
    int id = *(int*)arg;
    long local_reads = 0;

    for (int i = 0; i < OPERATIONS; i++) {
        // 获取读锁 (多个读者可同时持有)
        pthread_rwlock_rdlock(&db.rwlock);

        // 读取数据
        int value = db.data;
        local_reads++;
        (void)value;  // 使用数据，避免优化

        // 模拟读取处理
        if (i % 10000 == 0) {
            usleep(1);  // 偶尔休眠
        }

        pthread_rwlock_unlock(&db.rwlock);
    }

    printf("Reader %d completed %ld reads\n", id, local_reads);
    return NULL;
}

// 写者线程
void *writer_thread(void *arg) {
    int id = *(int*)arg;
    long local_writes = 0;

    for (int i = 0; i < OPERATIONS / 10; i++) {  // 写操作较少
        // 获取写锁 (独占)
        pthread_rwlock_wrlock(&db.rwlock);

        // 修改数据
        db.data = id * 1000 + i;
        db.write_count++;
        local_writes++;

        // 模拟写入处理
        usleep(10);

        pthread_rwlock_unlock(&db.rwlock);

        // 写之间休息
        usleep(100);
    }

    printf("Writer %d completed %ld writes\n", id, local_writes);
    return NULL;
}

// 尝试获取读写锁
void *try_rwlock_thread(void *arg) {
    (void)arg;
    int rd_success = 0, wr_success = 0;

    for (int i = 0; i < 1000; i++) {
        // 尝试获取读锁
        if (pthread_rwlock_tryrdlock(&db.rwlock) == 0) {
            rd_success++;
            pthread_rwlock_unlock(&db.rwlock);
        }

        // 尝试获取写锁
        if (pthread_rwlock_trywrlock(&db.rwlock) == 0) {
            wr_success++;
            pthread_rwlock_unlock(&db.rwlock);
        }
    }

    printf("Trylock: read=%d, write=%d\n", rd_success, wr_success);
    return NULL;
}

// 偏好设置
void demonstrate_preference(void) {
    pthread_rwlockattr_t attr;
    pthread_rwlock_t pref_rwlock;

    pthread_rwlockattr_init(&attr);

    // 设置读者优先 (默认)
    // pthread_rwlockattr_setkind_np(&attr, PTHREAD_RWLOCK_PREFER_READER_NP);

    // 设置写者优先
    #ifdef __linux__
    pthread_rwlockattr_setkind_np(&attr, PTHREAD_RWLOCK_PREFER_WRITER_NP);
    #endif

    pthread_rwlock_init(&pref_rwlock, &attr);

    printf("\n读写锁偏好设置完成\n");

    pthread_rwlock_destroy(&pref_rwlock);
    pthread_rwlockattr_destroy(&attr);
}

int main(void) {
    pthread_t readers[NUM_READERS];
    pthread_t writers[NUM_WRITERS];
    int reader_ids[NUM_READERS];
    int writer_ids[NUM_WRITERS];

    printf("=== 读写锁演示 ===\n");
    printf("Readers: %d, Writers: %d\n", NUM_READERS, NUM_WRITERS);

    // 创建读者线程
    for (int i = 0; i < NUM_READERS; i++) {
        reader_ids[i] = i;
        pthread_create(&readers[i], NULL, reader_thread, &reader_ids[i]);
    }

    // 创建写者线程
    for (int i = 0; i < NUM_WRITERS; i++) {
        writer_ids[i] = i;
        pthread_create(&writers[i], NULL, writer_thread, &writer_ids[i]);
    }

    // 等待所有线程完成
    for (int i = 0; i < NUM_READERS; i++) {
        pthread_join(readers[i], NULL);
    }
    for (int i = 0; i < NUM_WRITERS; i++) {
        pthread_join(writers[i], NULL);
    }

    printf("\nFinal data: %d, Write count: %d\n", db.data, db.write_count);

    demonstrate_preference();

    pthread_rwlock_destroy(&db.rwlock);
    return 0;
}
```

### 自旋锁 (Spinlock) 详解

```c
// spinlock_demo.c
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>

#define NUM_THREADS 4
#define ITERATIONS 10000000

typedef struct {
    pthread_spinlock_t spinlock;
    long counter;
} spin_data_t;

spin_data_t spin_data;

// 自旋锁线程 - 适用于短临界区
void *spinlock_worker(void *arg) {
    (void)arg;

    for (int i = 0; i < ITERATIONS; i++) {
        pthread_spin_lock(&spin_data.spinlock);

        // 极短的临界区 - 仅自增
        spin_data.counter++;

        pthread_spin_unlock(&spin_data.spinlock);
    }

    return NULL;
}

// 自定义自旋锁 (使用原子操作)
typedef struct {
    volatile int flag;
} custom_spinlock_t;

#define CUSTOM_SPINLOCK_INITIALIZER {0}

static inline void custom_spin_init(custom_spinlock_t *lock) {
    lock->flag = 0;
}

static inline void custom_spin_lock(custom_spinlock_t *lock) {
    // TAS (Test-And-Set) 自旋
    while (__sync_lock_test_and_set(&lock->flag, 1)) {
        // 自旋等待，可选CPU放松
        #ifdef __x86_64__
        __asm__ volatile ("pause" ::: "memory");
        #else
        sched_yield();  // 让出CPU
        #endif
    }
}

static inline void custom_spin_unlock(custom_spinlock_t *lock) {
    __sync_lock_release(&lock->flag);
}

custom_spinlock_t custom_lock = CUSTOM_SPINLOCK_INITIALIZER;
long custom_counter = 0;

void *custom_spin_worker(void *arg) {
    (void)arg;

    for (int i = 0; i < ITERATIONS; i++) {
        custom_spin_lock(&custom_lock);
        custom_counter++;
        custom_spin_unlock(&custom_lock);
    }

    return NULL;
}

// 带退避的自旋锁
#define MAX_BACKOFF 1024

typedef struct {
    volatile int flag;
} backoff_spinlock_t;

void backoff_spin_lock(backoff_spinlock_t *lock) {
    int backoff = 1;

    while (__sync_lock_test_and_set(&lock->flag, 1)) {
        // 指数退避
        for (int i = 0; i < backoff; i++) {
            #ifdef __x86_64__
            __asm__ volatile ("pause");
            #endif
        }
        backoff = (backoff < MAX_BACKOFF) ? backoff * 2 : MAX_BACKOFF;
    }
}

void backoff_spin_unlock(backoff_spinlock_t *lock) {
    __sync_lock_release(&lock->flag);
}

// 比较自旋锁和互斥锁性能
void performance_comparison(void) {
    pthread_t threads[NUM_THREADS];
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_spinlock_t spinlock;

    pthread_spin_init(&spinlock, PTHREAD_PROCESS_PRIVATE);

    long mutex_counter = 0;
    long spin_counter = 0;

    // 互斥锁测试
    // ... (省略计时代码)

    // 自旋锁测试
    // ... (省略计时代码)

    pthread_mutex_destroy(&mutex);
    pthread_spin_destroy(&spinlock);
}

int main(void) {
    pthread_t threads[NUM_THREADS];

    printf("=== 自旋锁演示 ===\n");

    // 初始化自旋锁
    pthread_spin_init(&spin_data.spinlock, PTHREAD_PROCESS_PRIVATE);
    spin_data.counter = 0;

    printf("Testing POSIX spinlock...\n");
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, spinlock_worker, NULL);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    printf("Final counter: %ld (expected: %d)\n",
           spin_data.counter, NUM_THREADS * ITERATIONS);

    pthread_spin_destroy(&spin_data.spinlock);

    // 自定义自旋锁测试
    printf("\nTesting custom spinlock...\n");
    custom_spin_init(&custom_lock);
    custom_counter = 0;

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, custom_spin_worker, NULL);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    printf("Custom counter: %ld\n", custom_counter);

    printf("\n自旋锁适用场景:\n");
    printf("- 临界区极短 (几个指令)\n");
    printf("- 多核系统\n");
    printf("- 锁竞争不激烈\n");
    printf("- 避免上下文切换开销\n");

    return 0;
}
```

### 信号量 (Semaphore) 详解

```c
// semaphore_demo.c
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 10
#define NUM_PRODUCERS 2
#define NUM_CONSUMERS 3
#define ITEMS_TO_PRODUCE 100

// 有界缓冲区
typedef struct {
    int buffer[BUFFER_SIZE];
    int in;
    int out;
} bounded_buffer_t;

bounded_buffer_t bb = {.in = 0, .out = 0};

// 信号量
sem_t empty;   // 空槽位计数
sem_t full;    // 已用槽位计数
sem_t mutex;   // 互斥访问缓冲区

// 计数信号量示例 - 资源池
#define MAX_CONNECTIONS 5
sem_t connection_pool;
int connections[MAX_CONNECTIONS];
int next_connection = 0;

int acquire_connection(void) {
    sem_wait(&connection_pool);  // P操作
    int conn = connections[next_connection++ % MAX_CONNECTIONS];
    printf("Acquired connection %d\n", conn);
    return conn;
}

void release_connection(int conn) {
    printf("Releasing connection %d\n", conn);
    sem_post(&connection_pool);  // V操作
}

void *producer(void *arg) {
    int id = *(int*)arg;

    for (int i = 0; i < ITEMS_TO_PRODUCE / NUM_PRODUCERS; i++) {
        int item = id * 1000 + i;

        sem_wait(&empty);   // 等待空槽位
        sem_wait(&mutex);   // 进入临界区

        // 生产数据
        bb.buffer[bb.in] = item;
        bb.in = (bb.in + 1) % BUFFER_SIZE;
        printf("Producer %d: produced %d\n", id, item);

        sem_post(&mutex);   // 离开临界区
        sem_post(&full);    // 增加已用槽位

        usleep(rand() % 10000);
    }

    return NULL;
}

void *consumer(void *arg) {
    int id = *(int*)arg;

    for (int i = 0; i < ITEMS_TO_PRODUCE / NUM_CONSUMERS; i++) {
        sem_wait(&full);    // 等待数据
        sem_wait(&mutex);   // 进入临界区

        // 消费数据
        int item = bb.buffer[bb.out];
        bb.out = (bb.out + 1) % BUFFER_SIZE;
        printf("Consumer %d: consumed %d\n", id, item);

        sem_post(&mutex);   // 离开临界区
        sem_post(&empty);   // 增加空槽位

        usleep(rand() % 15000);
    }

    return NULL;
}

// 二元信号量 (类似互斥锁)
sem_t binary_sem;
int shared_counter = 0;

void *binary_sem_worker(void *arg) {
    (void)arg;

    for (int i = 0; i < 100000; i++) {
        sem_wait(&binary_sem);  // P操作，类似lock
        shared_counter++;
        sem_post(&binary_sem);  // V操作，类似unlock
    }

    return NULL;
}

int main(void) {
    pthread_t producers[NUM_PRODUCERS];
    pthread_t consumers[NUM_CONSUMERS];
    int producer_ids[NUM_PRODUCERS];
    int consumer_ids[NUM_CONSUMERS];

    printf("=== 信号量演示 ===\n");

    // 初始化信号量
    sem_init(&empty, 0, BUFFER_SIZE);  // 初始空槽位=BUFFER_SIZE
    sem_init(&full, 0, 0);              // 初始已用槽位=0
    sem_init(&mutex, 0, 1);             // 二元信号量

    // 初始化连接池信号量
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        connections[i] = i + 1;
    }
    sem_init(&connection_pool, 0, MAX_CONNECTIONS);

    printf("\n1. 生产者-消费者模型\n");

    // 创建生产者
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        producer_ids[i] = i;
        pthread_create(&producers[i], NULL, producer, &producer_ids[i]);
    }

    // 创建消费者
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        consumer_ids[i] = i;
        pthread_create(&consumers[i], NULL, consumer, &consumer_ids[i]);
    }

    // 等待完成
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(producers[i], NULL);
    }
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_join(consumers[i], NULL);
    }

    printf("\n2. 连接池示例\n");
    int c1 = acquire_connection();
    int c2 = acquire_connection();
    release_connection(c1);
    release_connection(c2);

    // 清理
    sem_destroy(&empty);
    sem_destroy(&full);
    sem_destroy(&mutex);
    sem_destroy(&connection_pool);

    return 0;
}
```

### 条件变量 (Condition Variable) 详解

```c
// condvar_demo.c
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define BUFFER_SIZE 10
#define NUM_ITEMS 100

// 带条件变量的有界缓冲区
typedef struct {
    int buffer[BUFFER_SIZE];
    int count;
    int in;
    int out;

    pthread_mutex_t mutex;
    pthread_cond_t not_full;
    pthread_cond_t not_empty;
} cond_buffer_t;

cond_buffer_t cb = {
    .count = 0,
    .in = 0,
    .out = 0,
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .not_full = PTHREAD_COND_INITIALIZER,
    .not_empty = PTHREAD_COND_INITIALIZER
};

void cond_buffer_put(cond_buffer_t *b, int item) {
    pthread_mutex_lock(&b->mutex);

    // 等待直到有空间
    while (b->count == BUFFER_SIZE) {
        pthread_cond_wait(&b->not_full, &b->mutex);
    }

    b->buffer[b->in] = item;
    b->in = (b->in + 1) % BUFFER_SIZE;
    b->count++;

    printf("Produced: %d, count=%d\n", item, b->count);

    // 通知消费者
    pthread_cond_signal(&b->not_empty);
    pthread_mutex_unlock(&b->mutex);
}

int cond_buffer_get(cond_buffer_t *b) {
    pthread_mutex_lock(&b->mutex);

    // 等待直到有数据
    while (b->count == 0) {
        pthread_cond_wait(&b->not_empty, &b->mutex);
    }

    int item = b->buffer[b->out];
    b->out = (b->out + 1) % BUFFER_SIZE;
    b->count--;

    printf("Consumed: %d, count=%d\n", item, b->count);

    // 通知生产者
    pthread_cond_signal(&b->not_full);
    pthread_mutex_unlock(&b->mutex);

    return item;
}

// 广播示例 - 多消费者唤醒
typedef struct {
    int ready;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} barrier_t;

barrier_t barrier = {
    .ready = 0,
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .cond = PTHREAD_COND_INITIALIZER
};

void wait_for_ready(barrier_t *b) {
    pthread_mutex_lock(&b->mutex);
    while (!b->ready) {
        pthread_cond_wait(&b->cond, &b->mutex);
    }
    pthread_mutex_unlock(&b->mutex);
}

void set_ready(barrier_t *b) {
    pthread_mutex_lock(&b->mutex);
    b->ready = 1;
    pthread_cond_broadcast(&b->cond);  // 唤醒所有等待线程
    pthread_mutex_unlock(&b->mutex);
}

// 定时等待示例
int timed_wait_for_ready(barrier_t *b, int timeout_ms) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += timeout_ms / 1000;
    ts.tv_nsec += (timeout_ms % 1000) * 1000000;

    pthread_mutex_lock(&b->mutex);
    int result = 0;
    while (!b->ready && result == 0) {
        result = pthread_cond_timedwait(&b->cond, &b->mutex, &ts);
    }
    int was_ready = b->ready;
    pthread_mutex_unlock(&b->mutex);

    if (result == ETIMEDOUT) {
        printf("Timed out waiting\n");
        return -1;
    }
    return was_ready ? 0 : -1;
}

// 线程函数
void *producer_thread(void *arg) {
    (void)arg;
    for (int i = 0; i < NUM_ITEMS; i++) {
        cond_buffer_put(&cb, i);
        usleep(rand() % 10000);
    }
    return NULL;
}

void *consumer_thread(void *arg) {
    int id = *(int*)arg;
    for (int i = 0; i < NUM_ITEMS / 2; i++) {
        int item = cond_buffer_get(&cb);
        (void)item;
        usleep(rand() % 15000);
    }
    printf("Consumer %d finished\n", id);
    return NULL;
}

int main(void) {
    pthread_t producer;
    pthread_t consumers[2];
    int consumer_ids[2] = {1, 2};

    printf("=== 条件变量演示 ===\n");

    pthread_create(&producer, NULL, producer_thread, NULL);
    pthread_create(&consumers[0], NULL, consumer_thread, &consumer_ids[0]);
    pthread_create(&consumers[1], NULL, consumer_thread, &consumer_ids[1]);

    pthread_join(producer, NULL);
    pthread_join(consumers[0], NULL);
    pthread_join(consumers[1], NULL);

    printf("\n=== 广播测试 ===\n");
    // 创建等待线程
    // ... 省略

    pthread_mutex_destroy(&cb.mutex);
    pthread_cond_destroy(&cb.not_full);
    pthread_cond_destroy(&cb.not_empty);

    return 0;
}
```

## C11原子操作

### C11原子类型和操作

```c
// c11_atomics.c
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>

#define NUM_THREADS 4
#define ITERATIONS 10000000

// 原子计数器
atomic_int atomic_counter = 0;
atomic_long atomic_sum = 0;

// 内存序说明:
// memory_order_relaxed - 无同步或顺序约束
// memory_order_consume - 消费操作，数据依赖同步
// memory_order_acquire - 获取操作，用于加载
// memory_order_release - 释放操作，用于存储
// memory_order_acq_rel - 获取-释放，用于读-修改-写
// memory_order_seq_cst - 顺序一致，最强约束(默认)

// 基本原子操作
int atomic_worker(void *arg) {
    (void)arg;

    for (int i = 0; i < ITERATIONS; i++) {
        // 原子自增 (seq_cst)
        atomic_fetch_add(&atomic_counter, 1);

        // 原子自增 (relaxed)
        atomic_fetch_add_explicit(&atomic_sum, i, memory_order_relaxed);
    }

    return 0;
}

// 原子标志 (类似自旋锁)
atomic_flag spinlock = ATOMIC_FLAG_INIT;
int protected_data = 0;

void atomic_lock(void) {
    // 测试并设置，忙等待
    while (atomic_flag_test_and_set(&spinlock)) {
        // 可选: 让出CPU
        thrd_yield();
    }
}

void atomic_unlock(void) {
    atomic_flag_clear(&spinlock);
}

int spinlock_worker(void *arg) {
    (void)arg;

    for (int i = 0; i < ITERATIONS; i++) {
        atomic_lock();
        protected_data++;
        atomic_unlock();
    }

    return 0;
}

// 原子指针操作
_Atomic(void*) atomic_ptr = NULL;

void pointer_example(void) {
    int data = 42;

    // 原子存储指针
    atomic_store(&atomic_ptr, &data);

    // 原子加载指针
    int *p = atomic_load(&atomic_ptr);

    // 比较并交换
    int new_data = 100;
    int *expected = &data;
    atomic_compare_exchange_strong(&atomic_ptr, &expected, &new_data);
}

// 原子布尔
atomic_bool ready = ATOMIC_VAR_INIT(false);
atomic_int value = ATOMIC_VAR_INIT(0);

// 发布-订阅模式
void producer_thread(void) {
    value = 42;
    // 确保value写入在ready之前对其他线程可见
    atomic_store_explicit(&ready, true, memory_order_release);
}

void consumer_thread(void) {
    // 确保读取ready后再读取value
    while (!atomic_load_explicit(&ready, memory_order_acquire)) {
        thrd_yield();
    }
    printf("Value: %d\n", value);
}

// 原子浮点 (使用union实现)
typedef union {
    _Atomic(uint64_t) i;
    double d;
} atomic_double_t;

void atomic_double_store(atomic_double_t *ad, double val) {
    union { double d; uint64_t i; } u = {.d = val};
    atomic_store(&ad->i, u.i);
}

double atomic_double_load(atomic_double_t *ad) {
    uint64_t i = atomic_load(&ad->i);
    union { uint64_t i; double d; } u = {.i = i};
    return u.d;
}

int main(void) {
    thrd_t threads[NUM_THREADS];

    printf("=== C11 原子操作演示 ===\n");

    // 基本原子操作测试
    printf("\n1. 基本原子自增\n");
    atomic_counter = 0;
    atomic_sum = 0;

    for (int i = 0; i < NUM_THREADS; i++) {
        thrd_create(&threads[i], atomic_worker, NULL);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        thrd_join(threads[i], NULL);
    }

    printf("Atomic counter: %d (expected: %d)\n",
           atomic_load(&atomic_counter), NUM_THREADS * ITERATIONS);

    // 原子标志锁测试
    printf("\n2. 原子自旋锁\n");
    protected_data = 0;
    atomic_flag_clear(&spinlock);

    for (int i = 0; i < NUM_THREADS; i++) {
        thrd_create(&threads[i], spinlock_worker, NULL);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        thrd_join(threads[i], NULL);
    }

    printf("Protected data: %d (expected: %d)\n",
           protected_data, NUM_THREADS * ITERATIONS);

    printf("\nC11原子操作优势:\n");
    printf("- 标准可移植\n");
    printf("- 细粒度内存序控制\n");
    printf("- 无锁编程基础\n");
    printf("- 编译器可优化\n");

    return 0;
}
```

## 性能/适用场景/复杂度对比

### 对比矩阵: 各种同步机制的特性

| 特性 | 互斥锁 | 读写锁 | 自旋锁 | 信号量 | 条件变量 | C11原子 |
|------|--------|--------|--------|--------|----------|---------|
| 内存开销 | 中 | 中 | 低 | 低 | 中 | 最低 |
| 获取开销 | 高(阻塞) | 高(阻塞) | 低(忙等) | 中 | 高 | 最低 |
| 临界区长度 | 长 | 长 | 极短 | 任意 | N/A | 极短 |
| 多读支持 | 否 | 是 | 否 | 是 | N/A | 是(RCU) |
| 所有权 | 有 | 有 | 有 | 无 | N/A | 无 |
| 可递归 | 可选 | 否 | 否 | 是 | N/A | N/A |
| 跨进程 | 可选 | 可选 | 可选 | 是 | 否 | 可选 |
| 复杂度 | 低 | 中 | 低 | 中 | 高 | 高 |
| 死锁风险 | 中 | 中 | 低 | 高 | 高 | 低 |

### 性能对比表

| 场景 | 推荐机制 | 原因 |
|------|---------|------|
| 简单计数器 | C11原子 | 无锁，最高性能 |
| 链表插入/删除 | 互斥锁 | 临界区较长 |
| 配置读取 | 读写锁 | 读多写少 |
| 任务队列 | 条件变量+互斥锁 | 需要等待通知 |
| 资源池 | 信号量 | 需要计数功能 |
| 自增ID生成 | 原子操作 | 极短临界区 |
| 缓存行保护 | 自旋锁 | 多核，极短操作 |

## 死锁避免策略

```c
// deadlock_prevention.c
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
 * 死锁产生的四个必要条件:
 * 1. 互斥条件: 资源独占
 * 2. 请求与保持: 持有资源同时请求新资源
 * 3. 不剥夺条件: 资源只能主动释放
 * 4. 循环等待: 形成等待环路
 *
 * 预防策略: 破坏其中一个条件
 */

// 策略1: 锁排序 - 全局统一的加锁顺序
pthread_mutex_t lock_a = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_b = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_c = PTHREAD_MUTEX_INITIALIZER;

// 为每个锁分配唯一ID
#define LOCK_ID_A 1
#define LOCK_ID_B 2
#define LOCK_ID_C 3

// 有序加锁函数
void ordered_lock(pthread_mutex_t *lock1, int id1,
                  pthread_mutex_t *lock2, int id2) {
    if (id1 < id2) {
        pthread_mutex_lock(lock1);
        pthread_mutex_lock(lock2);
    } else {
        pthread_mutex_lock(lock2);
        pthread_mutex_lock(lock1);
    }
}

void ordered_unlock(pthread_mutex_t *lock1, pthread_mutex_t *lock2) {
    pthread_mutex_unlock(lock1);
    pthread_mutex_unlock(lock2);
}

// 策略2: 超时机制
trylock_with_timeout(pthread_mutex_t *lock1, pthread_mutex_t *lock2) {
    struct timespec timeout;
    clock_gettime(CLOCK_REALTIME, &timeout);
    timeout.tv_sec += 1;  // 1秒超时

    if (pthread_mutex_timedlock(lock1, &timeout) != 0) {
        return -1;  // 超时
    }

    if (pthread_mutex_timedlock(lock2, &timeout) != 0) {
        pthread_mutex_unlock(lock1);  // 释放已获取的锁
        return -1;
    }

    return 0;
}

// 策略3: 一次性获取所有锁 (银行家算法简化)
typedef struct {
    pthread_mutex_t *locks[3];
    int count;
} lock_set_t;

int acquire_all_or_none(lock_set_t *set) {
    int acquired = 0;

    // 尝试获取所有锁
    for (int i = 0; i < set->count; i++) {
        if (pthread_mutex_trylock(set->locks[i]) != 0) {
            // 获取失败，释放已获取的锁
            for (int j = 0; j < acquired; j++) {
                pthread_mutex_unlock(set->locks[j]);
            }
            return -1;
        }
        acquired++;
    }

    return 0;
}

// 策略4: 使用层级锁
#define LOCK_LEVEL_HIGH  10
#define LOCK_LEVEL_MED   20
#define LOCK_LEVEL_LOW   30

typedef struct {
    pthread_mutex_t mutex;
    int level;
} hierarchical_lock_t;

int hierarchical_lock(hierarchical_lock_t *current, hierarchical_lock_t *requested) {
    // 只能获取同级或更低级的锁
    if (requested->level < current->level) {
        fprintf(stderr, "Error: Cannot acquire higher level lock\n");
        return -1;
    }
    pthread_mutex_lock(&requested->mutex);
    return 0;
}

// 检测死锁 (运行时检查)
typedef struct {
    pthread_mutex_t *owned_locks[10];
    int count;
    pthread_t thread_id;
} lock_tracker_t;

_Thread_local lock_tracker_t tracker = {.count = 0};

void tracked_lock(pthread_mutex_t *lock) {
    // 检查是否会造成循环等待
    for (int i = 0; i < tracker.count; i++) {
        if (tracker.owned_locks[i] == lock) {
            fprintf(stderr, "Warning: Attempting to relock owned mutex\n");
        }
    }

    pthread_mutex_lock(lock);
    tracker.owned_locks[tracker.count++] = lock;
}

void tracked_unlock(pthread_mutex_t *lock) {
    pthread_mutex_unlock(lock);

    // 从追踪列表移除
    for (int i = 0; i < tracker.count; i++) {
        if (tracker.owned_locks[i] == lock) {
            tracker.owned_locks[i] = tracker.owned_locks[--tracker.count];
            break;
        }
    }
}

int main(void) {
    printf("=== 死锁预防策略 ===\n");
    printf("1. 锁排序: 全局统一的加锁顺序\n");
    printf("2. 超时机制: 避免无限等待\n");
    printf("3. 一次性获取: 要么全获取，要么全不获取\n");
    printf("4. 层级锁: 限制锁的获取顺序\n");
    printf("5. 运行时检测: 追踪锁的持有情况\n");
    return 0;
}
```

## 对比矩阵: 各种同步机制的特性

```
┌──────────────────────────────────────────────────────────────────────┐
│                    同步机制特性对比矩阵                              │
├──────────────┬────────┬────────┬────────┬────────┬────────┬─────────┤
│    特性      │ 互斥锁 │ 读写锁 │ 自旋锁 │ 信号量 │条件变量│C11原子  │
├──────────────┼────────┼────────┼────────┼────────┼────────┼─────────┤
│ 内存开销     │  40B   │  56B   │  4B    │  32B   │  48B   │  4-8B   │
├──────────────┼────────┼────────┼────────┼────────┼────────┼─────────┤
│ 获取延迟     │  ~100ns│  ~100ns│  ~10ns │  ~50ns │  ~100ns│  ~5ns   │
├──────────────┼────────┼────────┼────────┼────────┼────────┼─────────┤
│ 上下文切换   │   是   │   是   │   否   │   是   │   是   │   否    │
├──────────────┼────────┼────────┼────────┼────────┼────────┼─────────┤
│ 多读者支持   │   否   │   是   │   否   │   是   │   -    │   是    │
├──────────────┼────────┼────────┼────────┼────────┼────────┼─────────┤
│ 适用临界区   │  长    │  长    │  极短  │  任意  │   -    │  极短   │
├──────────────┼────────┼────────┼────────┼────────┼────────┼─────────┤
│ 编程复杂度   │   低   │   中   │   低   │   中   │   高   │   高    │
├──────────────┼────────┼────────┼────────┼────────┼────────┼─────────┤
│ 可移植性     │ POSIX  │ POSIX  │ POSIX  │ POSIX  │ POSIX  │ C11标准 │
├──────────────┼────────┼────────┼────────┼────────┼────────┼─────────┤
│ 推荐场景     │ 通用   │读多写少│ 多核短 │ 资源池 │事件通知│ 计数器  │
└──────────────┴────────┴────────┴────────┴────────┴────────┴─────────┘
```

## C代码示例: 各种锁的使用

```c
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
```

## 编译和运行

```bash
# Linux编译
gcc -std=c17 -pthread -o sync_demo complete_sync_example.c
gcc -std=c17 -pthread -o mutex_demo mutex_demo.c
gcc -std=c17 -pthread -o rwlock_demo rwlock_demo.c
gcc -std=c17 -pthread -o spinlock_demo spinlock_demo.c
gcc -std=c17 -pthread -o semaphore_demo semaphore_demo.c
gcc -std=c17 -pthread -o condvar_demo condvar_demo.c

# C11线程和原子 (需要较新编译器)
gcc -std=c17 -pthread -o c11_atomics c11_atomics.c

# 运行测试
./sync_demo
```
