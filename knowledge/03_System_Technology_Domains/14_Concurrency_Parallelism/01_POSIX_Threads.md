# POSIX线程编程

> **层级定位**: 03 System Technology Domains / 14 Concurrency_Parallelism
> **对应标准**: POSIX.1-2008 (IEEE Std 1003.1), pthreads
> **难度级别**: L3-L5
> **预估学习时间**: 8-10小时

---

## 📋 本节概要

| 属性 | 内容 |
|:-----|:-----|
| **核心概念** | 线程创建、同步、互斥、条件变量、线程安全 |
| **前置知识** | C基础、内存管理 |
| **后续延伸** | 进程间通信、异步I/O、并行计算 |
| **权威来源** | POSIX.1-2008, Butenhof《Programming with POSIX Threads》 |

---

## 📖 1. 线程基础

### 1.1 创建线程

```c
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// 线程函数
void *thread_function(void *arg) {
    int thread_num = *(int*)arg;
    printf("Thread %d: Hello from thread!\n", thread_num);

    // 线程返回值
    int *result = malloc(sizeof(int));
    *result = thread_num * 2;
    pthread_exit(result);
}

int main() {
    pthread_t threads[5];
    int thread_args[5];

    // 创建5个线程
    for (int i = 0; i < 5; i++) {
        thread_args[i] = i + 1;
        int rc = pthread_create(&threads[i], NULL,
                               thread_function, &thread_args[i]);
        if (rc != 0) {
            fprintf(stderr, "Error creating thread: %d\n", rc);
            return 1;
        }
    }

    // 等待所有线程完成
    for (int i = 0; i < 5; i++) {
        void *return_value;
        pthread_join(threads[i], &return_value);
        printf("Thread %d returned: %d\n", i + 1, *(int*)return_value);
        free(return_value);
    }

    printf("Main thread: All threads completed.\n");
    pthread_exit(NULL);
}
```

### 1.2 线程属性

```c
#include <pthread.h>

void thread_attributes() {
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    // 设置分离状态
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    // 设置栈大小
    size_t stack_size = 1024 * 1024;  // 1MB
    pthread_attr_setstacksize(&attr, stack_size);

    // 使用属性创建线程
    pthread_t thread;
    pthread_create(&thread, &attr, thread_func, NULL);

    pthread_attr_destroy(&attr);
}
```

---

## 📖 2. 互斥锁（Mutex）

### 2.1 基本互斥

```c
#include <pthread.h>
#include <stdio.h>

pthread_mutex_t mutex;
int shared_counter = 0;

void *increment_counter(void *arg) {
    for (int i = 0; i < 100000; i++) {
        pthread_mutex_lock(&mutex);
        shared_counter++;  // 临界区
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;

    // 初始化互斥锁
    pthread_mutex_init(&mutex, NULL);

    pthread_create(&t1, NULL, increment_counter, NULL);
    pthread_create(&t2, NULL, increment_counter, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Final counter: %d (expected: 200000)\n", shared_counter);

    pthread_mutex_destroy(&mutex);
    return 0;
}
```

### 2.2 读写锁

```c
#include <pthread.h>

pthread_rwlock_t rwlock;
int shared_data = 0;

void *reader(void *arg) {
    pthread_rwlock_rdlock(&rwlock);  // 多个读者可同时获取
    printf("Reader: data = %d\n", shared_data);
    pthread_rwlock_unlock(&rwlock);
    return NULL;
}

void *writer(void *arg) {
    pthread_rwlock_wrlock(&rwlock);  // 独占访问
    shared_data++;
    printf("Writer: updated data to %d\n", shared_data);
    pthread_rwlock_unlock(&rwlock);
    return NULL;
}
```

---

## 📖 3. 条件变量

```c
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int ready = 0;

void *producer(void *arg) {
    pthread_mutex_lock(&mutex);

    // 生产数据
    printf("Producer: producing data...\n");
    sleep(1);
    ready = 1;

    // 通知消费者
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);

    return NULL;
}

void *consumer(void *arg) {
    pthread_mutex_lock(&mutex);

    // 等待条件
    while (!ready) {
        pthread_cond_wait(&cond, &mutex);
        // 被唤醒后自动重新获取mutex
    }

    printf("Consumer: consuming data...\n");
    ready = 0;

    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main() {
    pthread_t prod, cons;

    pthread_create(&cons, NULL, consumer, NULL);
    sleep(0);  // 确保消费者先等待
    pthread_create(&prod, NULL, producer, NULL);

    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    return 0;
}
```

---

## 📖 4. 线程安全

```c
#include <pthread.h>
#include <stdio.h>

// 线程局部存储
__thread int thread_local_var = 0;  // GCC扩展
// 或 POSIX: pthread_key_t

// 可重入函数示例
int factorial_reentrant(int n, int *result) {
    if (n < 0) return -1;
    *result = 1;
    for (int i = 2; i <= n; i++) {
        *result *= i;
    }
    return 0;
}

// 线程安全的随机数生成
pthread_mutex_t rand_mutex = PTHREAD_MUTEX_INITIALIZER;

int thread_safe_rand() {
    pthread_mutex_lock(&rand_mutex);
    int r = rand();
    pthread_mutex_unlock(&rand_mutex);
    return r;
}
```

---

## ⚠️ 常见陷阱

### 陷阱 PTH01: 死锁

```c
// 死锁示例：锁顺序不一致
pthread_mutex_t m1, m2;

void thread_a() {
    pthread_mutex_lock(&m1);
    pthread_mutex_lock(&m2);  // 可能阻塞，如果thread_b持有m2
    // ...
    pthread_mutex_unlock(&m2);
    pthread_mutex_unlock(&m1);
}

void thread_b() {
    pthread_mutex_lock(&m2);
    pthread_mutex_lock(&m1);  // 可能阻塞，如果thread_a持有m1
    // ... 死锁！
}
```

### 陷阱 PTH02: 忘记解锁

```c
void dangerous() {
    pthread_mutex_lock(&mutex);
    if (error_condition) {
        return;  // 错误！mutex未解锁
    }
    pthread_mutex_unlock(&mutex);
}
```

---

## ✅ 质量验收清单

- [x] 线程创建与销毁
- [x] 互斥锁使用
- [x] 条件变量同步
- [x] 读写锁
- [x] 线程局部存储
- [x] 常见陷阱分析

---

> **更新记录**
>
> - 2025-03-09: 创建，补充核心缺失主题
