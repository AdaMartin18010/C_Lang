# 视频脚本：并发编程模式

> **视频标题**: 并发编程模式 - 从线程到无锁编程
> **视频时长**: 35分钟
> **目标受众**: 有C语言基础，希望掌握并发编程的开发者
> **前置知识**: C语言基础、指针、Linux基础
> **关联文档**: 系统编程 - 线程

---

## 开场 (0:00 - 1:30)

### 0:00 | 问题引入 | 画面：现代多核CPU芯片图，性能增长趋势

**讲解词**:
2005年后，CPU时钟速度增长放缓，处理器走向多核化。今天，从手机到服务器，多核已成为标配。但要充分发挥多核性能，程序必须能够并行执行——这就是并发编程的价值所在。

### 0:30 | 并发 vs 并行 | 画面：并发和并行的对比动画

**讲解词**:
首先区分两个概念。并发是处理多个任务的能力，任务可能交替执行；并行是同时执行多个任务，需要多核支持。C语言通过POSIX线程库提供标准的并发编程接口。

**概念对比**:

```
并发 (Concurrent):          并行 (Parallel):
┌──────────────────┐        ┌──────────────────┐
│ CPU 单核          │        │ CPU 多核          │
│ ┌────┐ ┌────┐    │        │ ┌────┐ ┌────┐    │
│ │ T1 │ │ T2 │    │        │ │ T1 │ │ T2 │    │
│ │████│ │░░░░│    │        │ │████│ │████│    │
│ │░░░░│ │████│    │        │ │████│ │████│    │
│ └────┘ └────┘    │        │ └────┘ └────┘    │
│ (交替执行)        │        │ (同时执行)        │
└──────────────────┘        └──────────────────┘
```

### 1:00 | 主题预告 | 画面：本集内容的思维导图

**讲解词**:
今天我们将学习POSIX线程的基础、同步原语、经典并发模式、性能优化技巧，以及无锁编程的基础知识。让我们开始并发之旅。

---

## 第一部分：POSIX线程基础 (1:30 - 7:00)

### 1:30 | 线程概念 | 画面：进程和线程的对比，内存共享示意

**讲解词**:
线程是进程内的执行单元。同一进程的所有线程共享代码段、数据段和堆，但每个线程有自己的栈和寄存器。这种共享使得线程间通信高效，但也带来了同步挑战。

**内存共享可视化**:

```
单进程多线程的内存布局:
┌─────────────────────────────────────┐
│ 代码段 (共享)                        │
├─────────────────────────────────────┤
│ 数据段 (共享)                        │
├─────────────────────────────────────┤
│ 堆 (共享)                           │
├─────────────────────────────────────┤
│ 线程A栈 │ 线程B栈 │ 线程C栈          │
├─────────────────────────────────────┤
│ 线程局部存储                         │
└─────────────────────────────────────┘
```

### 2:15 | 创建线程 | 画面：pthread_create函数的使用和参数说明

**讲解词**:
使用pthread_create创建线程。需要指定线程属性、线程函数和参数。线程函数返回void*，可以传递任意数据。

**代码演示**:

```c
#include <pthread.h>
#include <stdio.h>

void *thread_func(void *arg) {
    int num = *(int*)arg;
    printf("Thread %d running\n", num);
    return NULL;
}

int main() {
    pthread_t tid;
    int arg = 1;

    // 创建线程
    int ret = pthread_create(&tid, NULL, thread_func, &arg);
    if (ret != 0) {
        perror("pthread_create failed");
        return 1;
    }

    // 等待线程结束
    pthread_join(tid, NULL);
    return 0;
}
```

### 3:15 | 线程终止与清理 | 画面：线程终止的多种方式对比

**讲解词**:
线程可以通过多种方式终止：从线程函数返回、调用pthread_exit、被其他线程取消、或进程终止。重要资源应该使用清理处理程序确保释放。

**终止方式对比**:

```c
// 方式1: 正常返回
void *thread_func(void *arg) {
    // ... 工作
    return result;  // 返回值通过pthread_join获取
}

// 方式2: pthread_exit
void *thread_func(void *arg) {
    // ... 工作
    pthread_exit(result);  // 等效于return
}

// 方式3: 取消
pthread_cancel(tid);  // 请求取消线程

// 清理处理程序
void cleanup(void *arg) {
    free(arg);  // 确保资源释放
}

pthread_cleanup_push(cleanup, ptr);
// ... 可能取消的操作
pthread_cleanup_pop(1);
```

### 4:15 | 线程属性 | 画面：分离线程、栈大小等属性的设置

**讲解词**:
通过线程属性可以配置线程的各种参数。分离线程允许线程结束时自动回收资源；可以设置栈大小、调度策略等。

**属性设置**:

```c
pthread_attr_t attr;
pthread_attr_init(&attr);

// 设置分离状态
pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

// 设置栈大小 (默认通常8MB)
size_t stacksize = 1024 * 1024;  // 1MB
pthread_attr_setstacksize(&attr, stacksize);

// 创建线程
pthread_t tid;
pthread_create(&tid, &attr, thread_func, NULL);

pthread_attr_destroy(&attr);
```

### 5:00 | 线程局部存储 | 画面：__thread关键字和pthread_key的对比

**讲解词**:
线程局部存储让每个线程有自己的变量副本。GCC支持__thread关键字，更通用的方法是使用pthread_key。

**TLS实现方式**:

```c
// 方式1: GCC扩展
__thread int thread_local_var = 0;

// 方式2: POSIX标准
pthread_key_t key;

void init_tls() {
    pthread_key_create(&key, free);  // 可选的析构函数
}

void set_value(void *value) {
    pthread_setspecific(key, value);
}

void *get_value() {
    return pthread_getspecific(key);
}
```

### 5:50 | 多线程Hello World | 画面：完整的多线程程序演示

**讲解词**:
让我们看一个完整的多线程示例——并行计算数组和。创建多个线程，每个线程处理数组的一部分，最后汇总结果。

**完整示例**:

```c
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 4
#define ARRAY_SIZE 1000000

typedef struct {
    int *array;
    int start;
    int end;
    long long sum;
} ThreadData;

void *sum_partial(void *arg) {
    ThreadData *data = arg;
    data->sum = 0;
    for (int i = data->start; i < data->end; i++) {
        data->sum += data->array[i];
    }
    return NULL;
}

int main() {
    int *array = malloc(ARRAY_SIZE * sizeof(int));
    for (int i = 0; i < ARRAY_SIZE; i++) {
        array[i] = i + 1;
    }

    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];

    int chunk = ARRAY_SIZE / NUM_THREADS;
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i] = (ThreadData){
            .array = array,
            .start = i * chunk,
            .end = (i + 1) * chunk
        };
        pthread_create(&threads[i], NULL, sum_partial, &thread_data[i]);
    }

    long long total = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
        total += thread_data[i].sum;
    }

    printf("Total sum: %lld\n", total);
    free(array);
    return 0;
}
```

---

## 第二部分：同步原语 (7:00 - 15:00)

### 7:00 | 竞态条件 | 画面：竞态条件导致错误的数据动画

**讲解词**:
当多个线程访问共享数据且至少有一个是写操作时，就可能发生竞态条件。结果依赖于线程执行的精确时序，可能导致不可预测的错误。

**竞态条件可视化**:

```
初始: counter = 0

线程A:              线程B:
  read(0)             read(0)
  add 1               add 1
  write(1)            write(1)

结果: counter = 1 (期望: 2)
丢失了一次更新！
```

### 7:45 | 互斥锁 | 画面：互斥锁保护临界区的动画

**讲解词**:
互斥锁是最基本的同步原语。同一时间只有一个线程可以持有锁，其他线程必须等待。使用互斥锁保护临界区，确保对共享数据的原子访问。

**互斥锁使用**:

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int counter = 0;

void *increment(void *arg) {
    for (int i = 0; i < 100000; i++) {
        pthread_mutex_lock(&mutex);  // 获取锁
        counter++;                    // 临界区
        pthread_mutex_unlock(&mutex); // 释放锁
    }
    return NULL;
}
```

**锁的保护动画**:

```
时间线:
├─────┬─────┬─────┬─────┬─────┬─────┬─────┤
│ A获锁 │ A执行 │ A释锁 │ B获锁 │ B执行 │ B释锁 │
└─────┴─────┴─────┴─────┴─────┴─────┴─────┘

互斥性保证:
┌──────────────────────────────────────────────┐
│  线程A在临界区时，线程B被阻塞在lock()          │
└──────────────────────────────────────────────┘
```

### 8:45 | 死锁 | 画面：死锁的形成和避免策略

**讲解词**:
死锁发生在多个线程互相等待对方持有的资源时。避免死锁的策略包括：锁的顺序一致、使用trylock、限制持有时间。

**死锁示例**:

```c
// 死锁场景
pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m2 = PTHREAD_MUTEX_INITIALIZER;

// 线程A               线程B
pthread_mutex_lock(&m1);    pthread_mutex_lock(&m2);
pthread_mutex_lock(&m2);    pthread_mutex_lock(&m1);  // 死锁！
// ...                  // ...
pthread_mutex_unlock(&m2);  pthread_mutex_unlock(&m1);
pthread_mutex_unlock(&m1);  pthread_mutex_unlock(&m2);

// 解决方案: 统一获取顺序
// 总是先m1后m2
```

### 9:30 | 条件变量 | 画面：生产者-消费者问题的条件变量实现

**讲解词**:
条件变量允许线程等待特定条件。与互斥锁配合使用，可以实现高效的线程间通信。经典应用是生产者-消费者模式。

**生产者-消费者实现**:

```c
#define BUFFER_SIZE 100

int buffer[BUFFER_SIZE];
int count = 0, head = 0, tail = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

void produce(int item) {
    pthread_mutex_lock(&mutex);
    while (count == BUFFER_SIZE) {  // 必须用while
        pthread_cond_wait(&not_full, &mutex);
    }
    buffer[tail] = item;
    tail = (tail + 1) % BUFFER_SIZE;
    count++;
    pthread_cond_signal(&not_empty);
    pthread_mutex_unlock(&mutex);
}

int consume() {
    pthread_mutex_lock(&mutex);
    while (count == 0) {
        pthread_cond_wait(&not_empty, &mutex);
    }
    int item = buffer[head];
    head = (head + 1) % BUFFER_SIZE;
    count--;
    pthread_cond_signal(&not_full);
    pthread_mutex_unlock(&mutex);
    return item;
}
```

### 11:00 | 读写锁 | 画面：读写锁的并发读特性

**讲解词**:
读写锁允许多个读者同时访问，但写者独占。适合读多写少的场景，可以显著提高并发度。

**读写锁使用**:

```c
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
int data = 0;

void *reader(void *arg) {
    pthread_rwlock_rdlock(&rwlock);  // 多个读者可以同时持有
    printf("Read: %d\n", data);
    pthread_rwlock_unlock(&rwlock);
    return NULL;
}

void *writer(void *arg) {
    pthread_rwlock_wrlock(&rwlock);  // 独占访问
    data++;
    printf("Write: %d\n", data);
    pthread_rwlock_unlock(&rwlock);
    return NULL;
}
```

### 11:45 | 屏障 | 画面：屏障同步的动画

**讲解词**:
屏障用于同步多个线程的执行点。所有线程到达屏障后一起继续，适合分阶段并行算法。

**屏障使用**:

```c
pthread_barrier_t barrier;

void init() {
    pthread_barrier_init(&barrier, NULL, NUM_THREADS);
}

void *phase_work(void *arg) {
    // 阶段1
    do_phase1_work();
    pthread_barrier_wait(&barrier);  // 等待所有线程

    // 阶段2 (所有线程都完成了阶段1)
    do_phase2_work();
    pthread_barrier_wait(&barrier);

    return NULL;
}
```

### 12:30 | 信号量 | 画面：信号量的计数和等待机制

**讲解词**:
信号量是更通用的同步原语，可以控制同时访问某资源的线程数量。互斥锁是信号量的特例（计数为1）。

**信号量应用**:

```c
#include <semaphore.h>

sem_t sem;

void init() {
    // 初始计数为3，最多3个线程同时访问
    sem_init(&sem, 0, 3);
}

void limited_resource() {
    sem_wait(&sem);      // P操作，计数减1，为0时阻塞
    // 使用有限资源
    sem_post(&sem);      // V操作，计数加1
}
```

### 13:30 | 锁粒度选择 | 画面：粗粒度锁和细粒度锁的对比

**讲解词**:
锁粒度是并发设计的重要权衡。粗粒度锁简单但并发度低；细粒度锁并发度高但实现复杂、开销大。需要根据实际场景选择。

**粒度对比**:

```
粗粒度锁 (单锁):
┌─────────────────────────────────────┐
│ 锁A (保护所有数据)                   │
│ ┌─────┬─────┬─────┬─────┬─────┐    │
│ │数据1│数据2│数据3│数据4│数据5│    │
│ └─────┴─────┴─────┴─────┴─────┘    │
└─────────────────────────────────────┘
简单，但并发度低

细粒度锁 (多锁):
┌─────┐ ┌─────┐ ┌─────┐ ┌─────┐ ┌─────┐
│锁1  │ │锁2  │ │锁3  │ │锁4  │ │锁5  │
│数据1│ │数据2│ │数据3│ │数据4│ │数据5│
└─────┘ └─────┘ └─────┘ └─────┘ └─────┘
复杂，但并发度高
```

---

## 第三部分：并发模式 (15:00 - 22:00)

### 15:00 | 线程池模式 | 画面：线程池的架构和任务分发

**讲解词**:
线程池预先创建一组线程，任务提交到队列由工作线程处理。避免了频繁创建销毁线程的开销，可以控制并发度。

**线程池实现**:

```c
typedef struct {
    void (*function)(void *);
    void *argument;
} Task;

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    Task *queue;
    int queue_size;
    int head, tail, count;
    pthread_t *threads;
    int num_threads;
    int shutdown;
} ThreadPool;

void *worker(void *arg) {
    ThreadPool *pool = arg;
    while (1) {
        pthread_mutex_lock(&pool->mutex);
        while (pool->count == 0 && !pool->shutdown) {
            pthread_cond_wait(&pool->cond, &pool->mutex);
        }
        if (pool->shutdown) {
            pthread_mutex_unlock(&pool->mutex);
            break;
        }
        Task task = pool->queue[pool->head];
        pool->head = (pool->head + 1) % pool->queue_size;
        pool->count--;
        pthread_mutex_unlock(&pool->mutex);

        task.function(task.argument);  // 执行任务
    }
    return NULL;
}
```

### 16:45 | 读写锁模式 | 画面：RCU机制的简化说明

**讲解词**:
读-复制-更新(RCU)是一种高性能的读多写少同步机制。读者无锁访问，写者复制修改后原子更新指针。Linux内核广泛使用RCU。

**RCU简化实现**:

```c
// 简化版RCU思想
struct Data {
    int value;
};

_Atomic(struct Data*) global_data;

// 读者 (无锁)
int read_data() {
    struct Data *ptr = atomic_load(&global_data);
    return ptr->value;  // 安全读取
}

// 写者
void write_data(int new_val) {
    struct Data *old = atomic_load(&global_data);
    struct Data *new = malloc(sizeof(struct Data));
    *new = *old;          // 复制
    new->value = new_val; // 修改
    atomic_store(&global_data, new);  // 原子更新
    // 延迟释放old (需要等待读者退出)
}
```

### 18:00 | 工作窃取模式 | 画面：工作窃取的双端队列

**讲解词**:
工作窃取是负载均衡的高级技术。每个线程有自己的任务队列，空闲线程从其他线程队列"窃取"任务。Cilk和OpenMP使用这种技术。

**工作窃取示意**:

```
线程A (忙碌)          线程B (空闲)
┌─────────────┐      ┌─────────────┐
│ 任务队列      │      │ 任务队列      │
│ head    tail │      │ head    tail │
│  ↓       ↓   │      │  ↓       ↓   │
│ [1][2][3][4] │      │ [ ]        │
└─────────────┘      └─────────────┘
       ▲                    │
       │ 从尾部窃取           │
       └────────────────────┘
```

### 19:00 | Future/Promise模式 | 画面：异步任务和结果获取

**讲解词**:
Future/Promise模式将任务的提交和结果的获取分离。提交任务返回Future对象，可以在需要时阻塞获取结果，实现异步编程。

**模式实现**:

```c
typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    void *result;
    int ready;
} Future;

typedef struct {
    void *(*func)(void *);
    void *arg;
    Future *future;
} AsyncTask;

void *async_execute(void *arg) {
    AsyncTask *task = arg;
    void *result = task->func(task->arg);

    pthread_mutex_lock(&task->future->mutex);
    task->future->result = result;
    task->future->ready = 1;
    pthread_cond_broadcast(&task->future->cond);
    pthread_mutex_unlock(&task->future->mutex);

    free(task);
    return NULL;
}

void *future_get(Future *f) {
    pthread_mutex_lock(&f->mutex);
    while (!f->ready) {
        pthread_cond_wait(&f->cond, &f->mutex);
    }
    void *result = f->result;
    pthread_mutex_unlock(&f->mutex);
    return result;
}
```

### 20:15 | Actor模式 | 画面：Actor模型的消息传递

**讲解词**:
Actor模型是完全避免共享状态的并发范式。每个Actor有自己的状态和行为，通过异步消息通信。Erlang和Akka使用这种模型。

**Actor概念**:

```
Actor模型:
┌─────────┐        消息队列         ┌─────────┐
│ Actor A │◄──────┐          ┌──────►│ Actor B │
│ - 状态   │       │          │       │ - 状态   │
│ - 行为   │       ▼          ▼       │ - 行为   │
│ - 邮箱   │    ┌────────┐  ┌────────┤│ - 邮箱   │
└─────────┘    │ 消息1  │  │ 消息2  │└─────────┘
               │ 消息3  │  │ 消息4  │
               └────────┘  └────────┘

特性:
- 无共享状态
- 通过消息通信
- 每个Actor串行处理消息
- 位置透明
```

---

## 第四部分：原子操作与内存序 (22:00 - 28:00)

### 22:00 | 原子操作基础 | 画面：原子操作与普通操作的区别

**讲解词**:
原子操作是不可分割的操作，不会被其他线程看到中间状态。C11标准引入了stdatomic.h，提供标准化的原子操作。

**原子操作示例**:

```c
#include <stdatomic.h>

_Atomic int counter = 0;

void *increment(void *arg) {
    for (int i = 0; i < 100000; i++) {
        atomic_fetch_add(&counter, 1);  // 原子加1
    }
    return NULL;
}
```

### 22:45 | 内存序 | 画面：不同内存序的可见性保证

**讲解词**:
内存序定义了原子操作的同步强度。顺序一致性最强但最慢；获取-释放适合大多数同步场景；松弛序最快但保证最少。

**内存序对比**:

```
内存序强度:
┌──────────────────────────────────────────────────────┐
│ memory_order_seq_cst  (顺序一致性)                    │
│ - 所有线程看到一致的顺序                              │
│ - 最慢                                               │
├──────────────────────────────────────────────────────┤
│ memory_order_acquire/release                          │
│ - 获取: 之后的读写不会重排到之前                      │
│ - 释放: 之前的读写不会重排到之后                      │
│ - 适合锁的实现                                        │
├──────────────────────────────────────────────────────┤
│ memory_order_relaxed                                 │
│ - 仅保证原子性                                        │
│ - 无同步保证                                          │
│ - 最快                                               │
└──────────────────────────────────────────────────────┘
```

### 23:45 | 实现自旋锁 | 画面：自旋锁的代码和适用场景

**讲解词**:
自旋锁是简单的锁实现，等待时忙等循环。适用于锁持有时间极短的场景，避免了线程切换开销。

**自旋锁实现**:

```c
typedef atomic_flag SpinLock;

#define SPIN_LOCK_INIT ATOMIC_FLAG_INIT

void spin_lock(SpinLock *lock) {
    while (atomic_flag_test_and_set_explicit(lock,
                                              memory_order_acquire)) {
        // 自旋等待
    }
}

void spin_unlock(SpinLock *lock) {
    atomic_flag_clear_explicit(lock, memory_order_release);
}

// 使用
SpinLock lock = SPIN_LOCK_INIT;

spin_lock(&lock);
// 临界区
spin_unlock(&lock);
```

### 24:30 | 无锁数据结构基础 | 画面：CAS操作的动画

**讲解词**:
比较并交换(CAS)是实现无锁数据结构的基础操作。它原子地比较内存值和期望值，如果相等则更新为新值，返回是否成功。

**CAS操作**:

```c
// 原子CAS伪代码
bool atomic_compare_exchange(_Atomic int *ptr,
                              int *expected,
                              int desired) {
    if (*ptr == *expected) {
        *ptr = desired;
        return true;
    } else {
        *expected = *ptr;
        return false;
    }
}

// 实际使用
_Atomic int value = 0;
int expected = 0;
while (!atomic_compare_exchange_weak(&value, &expected, 1)) {
    expected = 0;  // 重试
}
```

### 25:15 | 无锁栈实现 | 画面：Treiber栈的算法动画

**讲解词**:
Treiber栈是最简单的无锁数据结构。使用CAS操作原子地更新栈顶指针，实现无锁的push和pop。

**Treiber栈**:

```c
typedef struct Node {
    void *data;
    struct Node *next;
} Node;

typedef struct {
    _Atomic(Node*) top;
} LockFreeStack;

void push(LockFreeStack *stack, void *data) {
    Node *new_node = malloc(sizeof(Node));
    new_node->data = data;

    do {
        new_node->next = atomic_load(&stack->top);
    } while (!atomic_compare_exchange_weak(&stack->top,
                                           &new_node->next,
                                           new_node));
}

void *pop(LockFreeStack *stack) {
    Node *top;
    do {
        top = atomic_load(&stack->top);
        if (top == NULL) return NULL;
    } while (!atomic_compare_exchange_weak(&stack->top,
                                           &top,
                                           top->next));
    void *data = top->data;
    free(top);  // 注意：ABA问题
    return data;
}
```

### 26:30 | ABA问题 | 画面：ABA问题的场景和解决方案

**讲解词**:
ABA问题是无锁编程的经典陷阱。CAS检查时值相同，但可能中间经历了A→B→A的变化，导致错误。解决方案包括使用版本号或Hazard Pointer。

**ABA问题**:

```
ABA问题场景:
时间: T1        T2        T3
     │         │         │
     ▼         ▼         ▼
栈顶: A    →   B    →    A   (值相同但对象不同!)
      │               ▲
      └───────────────┘
      线程1读取A, 被抢占
      线程2弹出A, 推入B, 弹出B, 推入A
      线程1恢复, CAS成功但栈已改变

解决方案:
- 使用DCAS (双字CAS, 包含版本号)
- Hazard Pointer (延迟回收)
- 读-Copy-更新 (RCU)
```

---

## 第五部分：性能优化与调试 (28:00 - 33:00)

### 28:00 | 假共享 | 画面：缓存行级别的竞争

**讲解词**:
假共享发生在不同线程修改同一缓存行的不同变量时。虽然逻辑上不冲突，但硬件缓存一致性协议会导致性能大幅下降。

**假共享问题**:

```
缓存行 (64字节):
┌────────────────────────────────────────────────────────────────┐
│ 变量A (线程1修改) │ 变量B (线程2修改) │ ...                    │
└────────────────────────────────────────────────────────────────┘
        │                    │
        ▼                    ▼
    CPU 0缓存             CPU 1缓存
    (修改A使B失效)        (修改B使A失效)

结果: 频繁的缓存同步，性能下降

解决方案: 填充到缓存行边界
struct PaddedCounter {
    atomic_int count;
    char padding[64 - sizeof(atomic_int)];  // 填充
};
```

### 29:00 | ThreadSanitizer | 画面：TSan检测数据竞争的输出

**讲解词**:
ThreadSanitizer是检测数据竞争的强大工具。编译时添加-fsanitize=thread，运行时会报告所有检测到的问题。

**TSan使用**:

```bash
# 编译
gcc -fsanitize=thread -g -o program program.c -pthread

# 运行
./program

# 输出示例
WARNING: ThreadSanitizer: data race
  Write of size 4 at 0x7ffd1234 by thread T1:
    #0 thread_func program.c:15
  Previous read of size 4 at 0x7ffd1234 by thread T2:
    #0 main program.c:25
```

### 30:00 | 性能分析 | 画面：perf和火焰图分析多线程程序

**讲解词**:
perf是Linux性能分析利器。可以分析CPU使用率、缓存命中率、锁竞争等。结合火焰图可以直观定位热点。

**perf命令**:

```bash
# 记录性能数据
perf record -g ./program

# 生成报告
perf report

# 分析缓存命中率
perf stat -e cache-misses,cache-references ./program

# 生成火焰图
perf script | stackcollapse-perf.pl | flamegraph.pl > flame.svg
```

### 31:00 | 并发最佳实践 | 画面：最佳实践的代码示例

**讲解词**:
最后总结并发编程的最佳实践：最小化共享状态、优先使用高层抽象、避免过早优化、充分测试并发场景。

**最佳实践**:

```c
// 1. 减少锁持有时间
pthread_mutex_lock(&mutex);
int local_copy = shared_data;  // 快速复制
pthread_mutex_unlock(&mutex);
process(local_copy);  // 长时间处理在锁外

// 2. 使用读写锁优化读多写少
pthread_rwlock_rdlock(&rwlock);  // 多个读者
read_data();
pthread_rwlock_unlock(&rwlock);

// 3. 避免在持有锁时调用外部代码
pthread_mutex_lock(&mutex);
// 不要在这里调用可能阻塞的函数
pthread_mutex_unlock(&mutex);

// 4. 使用原子操作替代锁 (简单场景)
atomic_fetch_add(&counter, 1);  // 比锁更高效
```

### 32:00 | 调试技巧 | 画面：常见并发Bug的调试方法

**讲解词**:
并发Bug难以重现和调试。技巧包括：使用专门工具、添加调试输出、强制触发竞争、代码审查。

**调试技巧清单**:

- 使用Helgrind/DRD/TSan检测竞争
- 打印线程ID和时间戳跟踪执行
- 使用usleep()强制特定执行顺序
- 代码审查寻找缺少锁保护的地方
- 压力测试增加Bug触发概率

---

## 总结 (33:00 - 35:00)

### 33:00 | 核心概念回顾 | 画面：知识体系的思维导图

**讲解词**:
让我们回顾今天的内容。我们学习了POSIX线程基础、同步原语、并发设计模式、原子操作和内存序，以及性能优化和调试技巧。

**知识总结**:

```
并发编程模式
├── 线程基础
│   ├── 线程创建和终止
│   ├── 线程属性和分离
│   └── 线程局部存储
├── 同步原语
│   ├── 互斥锁
│   ├── 条件变量
│   ├── 读写锁
│   └── 屏障和信号量
├── 并发模式
│   ├── 线程池
│   ├── 工作窃取
│   ├── Future/Promise
│   └── Actor模型
├── 无锁编程
│   ├── 原子操作
│   ├── 内存序
│   └── CAS和无锁数据结构
└── 性能优化
    ├── 避免假共享
    ├── 工具检测
    └── 最佳实践
```

### 34:00 | 结束 | 画面：资源链接和下集预告

**讲解词**:
感谢观看！本视频代码示例可以在GitHub找到。下一集我们将学习MISRA C和安全编码规范。敬请期待！

---

## 技术备注

### 编译要求

```bash
gcc -std=c11 -pthread -o program program.c
```

### 检测工具

- ThreadSanitizer: `-fsanitize=thread`
- Helgrind: `valgrind --tool=helgrind ./program`

### 延伸阅读

- [POSIX Threads Programming](https://computing.llnl.gov/tutorials/pthreads/)
- [C++ Concurrency in Action](https://www.manning.com/books/c-plus-plus-concurrency-in-action)

---

*脚本版本: 1.0*
*最后更新: 2026-03-19*
