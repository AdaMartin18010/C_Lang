# MISRA C:2023 Rules 9.1-9.5 - 资源管理与并发

> 资源获取释放、并发安全、线程同步

---

## 规则分类概述

| 规则编号 | 规则名称 | 强制性 | 核心要求 |
|:--------:|:---------|:------:|:---------|
| Rule 9.1 | 资源获取 | Required | 明确资源所有权 |
| Rule 9.2 | 资源释放 | Required | 确保资源释放 |
| Rule 9.3 | 信号处理 | Required | 信号处理安全 |
| Rule 9.4 | 并发访问 | Required | 数据竞争防护 |
| Rule 9.5 | 同步原语 | Required | 同步机制安全 |

---


---

## 📑 目录

- [MISRA C:2023 Rules 9.1-9.5 - 资源管理与并发](#misra-c2023-rules-91-95---资源管理与并发)
  - [规则分类概述](#规则分类概述)
  - [📑 目录](#-目录)
  - [Rule 9.1 - 资源获取](#rule-91---资源获取)
    - [规则原文](#规则原文)
    - [违反示例](#违反示例)
    - [合规示例](#合规示例)
  - [Rule 9.2 - 资源释放](#rule-92---资源释放)
    - [规则原文](#规则原文-1)
    - [资源所有权模式](#资源所有权模式)
  - [Rule 9.3 - 信号处理](#rule-93---信号处理)
    - [规则原文](#规则原文-2)
    - [安全的信号处理](#安全的信号处理)
    - [信号处理禁忌](#信号处理禁忌)
  - [Rule 9.4 - 并发访问](#rule-94---并发访问)
    - [规则原文](#规则原文-3)
    - [违反示例](#违反示例-1)
    - [合规示例](#合规示例-1)
  - [Rule 9.5 - 同步原语](#rule-95---同步原语)
    - [规则原文](#规则原文-4)
    - [死锁防护](#死锁防护)
    - [条件变量安全](#条件变量安全)
  - [并发安全检查清单](#并发安全检查清单)


---

## Rule 9.1 - 资源获取

### 规则原文

> **Rule 9.1 (Required)**: Resources shall be acquired and released at the same level of abstraction.
>
> **中文**: 资源应在同一抽象级别获取和释放。

### 违反示例

```c
/* ❌ 违反 - 不同级别的资源管理 */
void low_level_open(void)
{
    fp = fopen("file", "r");  /* 底层打开 */
}

void high_level_process(void)
{
    low_level_open();
    /* 处理 */
    fclose(fp);  /* 高层关闭 - 不匹配！ */
}

/* ❌ 违反 - 分散的资源管理 */
void init_system(void)
{
    mutex1 = create_mutex();
    mutex2 = create_mutex();
    /* ... 其他初始化 */
}

void cleanup_partial(void)
{
    destroy_mutex(mutex1);  /* 只释放部分资源 */
}
```

### 合规示例

```c
/* ✅ 合规 - RAII模式 */
struct file_handle {
    FILE *fp;
};

struct file_handle *file_open(const char *path)
{
    struct file_handle *h = malloc(sizeof(*h));
    if (h == NULL) {
        return NULL;
    }
    h->fp = fopen(path, "r");
    if (h->fp == NULL) {
        free(h);
        return NULL;
    }
    return h;
}

void file_close(struct file_handle *h)
{
    if (h != NULL) {
        if (h->fp != NULL) {
            fclose(h->fp);
        }
        free(h);
    }
}

/* ✅ 合规 - 对称的资源管理 */
int acquire_resources(struct resources *res)
{
    res->fd = open_device();
    if (res->fd < 0) {
        return -1;
    }

    res->buffer = malloc(BUF_SIZE);
    if (res->buffer == NULL) {
        close_device(res->fd);  /* 释放已获取的资源 */
        return -1;
    }

    return 0;
}

void release_resources(struct resources *res)
{
    if (res->buffer != NULL) {
        free(res->buffer);
        res->buffer = NULL;
    }
    if (res->fd >= 0) {
        close_device(res->fd);
        res->fd = -1;
    }
}
```

---

## Rule 9.2 - 资源释放

### 规则原文

> **Rule 9.2 (Required)**: All resources allocated by a module shall be released by the same module.
>
> **中文**: 模块分配的所有资源必须由同一模块释放。

### 资源所有权模式

```c
/* ✅ 合规 - 所有权明确 */

/* 模块A - 分配 */
void *module_a_alloc(size_t size)
{
    return malloc(size);
}

/* 模块A - 释放 */
void module_a_free(void *ptr)
{
    free(ptr);
}

/* 模块B - 使用但不释放 */
void module_b_use(void)
{
    void *data = module_a_alloc(100);
    /* 使用data */
    module_a_free(data);  /* 或者转移所有权 */
}

/* ✅ 合规 - 所有权转移文档化 */
/**
 * @brief Create a connection object
 * @return Ownership transferred to caller
 */
connection_t *connection_create(void);

/**
 * @brief Destroy a connection object
 * @param conn Ownership transferred from caller
 */
void connection_destroy(connection_t *conn);
```

---

## Rule 9.3 - 信号处理

### 规则原文

> **Rule 9.3 (Required)**: Signal handlers shall not perform complex operations.
>
> **中文**: 信号处理程序不得执行复杂操作。

### 安全的信号处理

```c
/* ✅ 合规 - 安全的信号处理 */
#include <signal.h>
#include <stdatomic.h>

/* 使用原子标志 */
static volatile sig_atomic_t signal_received = 0;

/* 信号处理程序 - 只做最少的操作 */
void safe_signal_handler(int sig)
{
    (void)sig;
    signal_received = 1;  /* 只设置标志 */
}

/* 主循环中处理 */
void main_loop(void)
{
    signal(SIGTERM, safe_signal_handler);
    signal(SIGINT, safe_signal_handler);

    while (!signal_received) {
        /* 正常工作 */
        process_tasks();
    }

    /* 优雅退出 */
    cleanup_and_exit();
}
```

### 信号处理禁忌

```c
/* ❌ 严重违反 - 信号处理中做太多操作 */
void unsafe_handler(int sig)
{
    /* 这些在信号处理程序中都是未定义行为！ */
    printf("Signal received\n");  /* 非异步信号安全 */
    free(global_ptr);             /* 可能破坏堆 */
    fclose(file);                 /* 非异步信号安全 */
    malloc(100);                  /* 非异步信号安全 */
}

/* ✅ 合规 - 异步信号安全函数列表 */
/* _Exit, _exit, abort, accept, access, alarm, bind, cfgetispeed,
 * cfgetospeed, cfsetispeed, cfsetospeed, chdir, chmod, chown,
 * clock_gettime, close, connect, creat, dup, dup2, execl, execle,
 * execv, execve, faccessat, fchdir, fchmod, fchmodat, fchown,
 * fchownat, fcntl, fdatasync, fexecve, ffs, fork, fstat, fstatat,
 * fsync, ftruncate, futimens, getegid, geteuid, getgid, getgroups,
 * getpeername, getpgrp, getpid, getppid, getsockname, getsockopt,
 * getuid, htonl, htons, inet_addr, inet_ntoa, ioctl, kill, link,
 * linkat, listen, longjmp, lseek, lstat, memccpy, memchr, memcmp,
 * memcpy, memmove, memset, mkdir, mkdirat, mkfifo, mkfifoat, mknod,
 * mknodat, ntohl, ntohs, open, openat, pause, pipe, poll, posix_trace_event,
 * pselect, pthread_kill, pthread_self, pthread_sigmask, raise, read,
 * readlink, readlinkat, recv, recvfrom, recvmsg, rename, renameat,
 * rmdir, select, sem_post, send, sendmsg, sendto, setgid, setpgid,
 * setsid, setsockopt, setuid, shutdown, sigaction, sigaddset, sigdelset,
 * sigemptyset, sigfillset, sigismember, signal, sigpause, sigpending,
 * sigprocmask, sigqueue, sigset, sigsuspend, sleep, sockatmark, socket,
 * socketpair, stat, stpcpy, stpncpy, strcat, strchr, strcmp, strcpy,
 * strcspn, strlen, strncat, strncmp, strncpy, strnlen, strpbrk, strrchr,
 * strspn, strstr, strtok_r, strxfrm, symlink, symlinkat, tcdrain,
 * tcflow, tcflush, tcgetattr, tcgetpgrp, tcsendbreak, tcsetattr,
 * tcsetpgrp, time, timer_getoverrun, timer_gettime, timer_settime,
 * times, umask, uname, unlink, unlinkat, utime, utimensat, utimes,
 * wait, waitpid, write
 */
```

---

## Rule 9.4 - 并发访问

### 规则原文

> **Rule 9.4 (Required)**: Shared objects shall be protected against concurrent access.
>
> **中文**: 共享对象必须防止并发访问。

### 违反示例

```c
/* ❌ 违反 - 无保护的共享数据 */
int shared_counter = 0;

void increment_unsafe(void)
{
    shared_counter++;  /* 数据竞争！ */
}

/* ❌ 违反 - 检查-使用竞争 */
if (shared_ptr != NULL) {
    /* 另一个线程可能在这里释放 */
    use(shared_ptr);  /* 使用已释放内存 */
}
```

### 合规示例

```c
/* ✅ 合规 - 互斥锁保护 */
#include <pthread.h>

static int shared_counter = 0;
static pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

void increment_safe(void)
{
    pthread_mutex_lock(&counter_mutex);
    shared_counter++;
    pthread_mutex_unlock(&counter_mutex);
}

/* ✅ 合规 - 原子操作 */
#include <stdatomic.h>

static _Atomic(int) atomic_counter = 0;

void increment_atomic(void)
{
    atomic_fetch_add(&atomic_counter, 1, memory_order_relaxed);
}

/* ✅ 合规 - 读写锁 */
static pthread_rwlock_t data_rwlock = PTHREAD_RWLOCK_INITIALIZER;
static struct data shared_data;

void read_data(struct data *out)
{
    pthread_rwlock_rdlock(&data_rwlock);
    *out = shared_data;
    pthread_rwlock_unlock(&data_rwlock);
}

void write_data(const struct data *in)
{
    pthread_rwlock_wrlock(&data_rwlock);
    shared_data = *in;
    pthread_rwlock_unlock(&data_rwlock);
}
```

---

## Rule 9.5 - 同步原语

### 规则原文

> **Rule 9.5 (Required)**: Synchronization primitives shall be used correctly.
>
> **中文**: 同步原语必须正确使用。

### 死锁防护

```c
/* ❌ 违反 - 死锁风险 */
void deadlock_risk(void)
{
    pthread_mutex_lock(&mutex_a);
    pthread_mutex_lock(&mutex_b);  /* 如果另一线程顺序相反，死锁！ */
    /* ... */
    pthread_mutex_unlock(&mutex_b);
    pthread_mutex_unlock(&mutex_a);
}

/* ✅ 合规 - 统一的锁顺序 */
#define LOCK_ORDER_A 1
#define LOCK_ORDER_B 2

void safe_lock_order(void)
{
    /* 始终按相同顺序获取 */
    pthread_mutex_lock(&mutex_a);  /* 顺序A */
    pthread_mutex_lock(&mutex_b);  /* 顺序B */
    /* ... */
    pthread_mutex_unlock(&mutex_b);
    pthread_mutex_unlock(&mutex_a);
}

/* ✅ 合规 - 尝试锁 */
void try_lock_pattern(void)
{
    pthread_mutex_lock(&mutex_a);
    if (pthread_mutex_trylock(&mutex_b) != 0) {
        pthread_mutex_unlock(&mutex_a);
        return;  /* 稍后重试 */
    }
    /* 成功获取两个锁 */
    /* ... */
    pthread_mutex_unlock(&mutex_b);
    pthread_mutex_unlock(&mutex_a);
}
```

### 条件变量安全

```c
/* ✅ 合规 - 条件变量使用模式 */
static pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t queue_cond = PTHREAD_COND_INITIALIZER;
static struct queue work_queue;

void producer(struct work_item *item)
{
    pthread_mutex_lock(&queue_mutex);
    queue_push(&work_queue, item);
    pthread_cond_signal(&queue_cond);  /* 通知消费者 */
    pthread_mutex_unlock(&queue_mutex);
}

void consumer(void)
{
    pthread_mutex_lock(&queue_mutex);
    while (queue_empty(&work_queue)) {
        pthread_cond_wait(&queue_cond, &queue_mutex);  /* 原子释放锁并等待 */
    }
    struct work_item *item = queue_pop(&work_queue);
    pthread_mutex_unlock(&queue_mutex);

    process(item);
}
```

---

## 并发安全检查清单

```markdown
□ 所有共享数据有同步保护
□ 锁的获取释放配对
□ 统一的锁获取顺序（防死锁）
□ 信号处理程序极简
□ 条件变量配合while使用（防虚假唤醒）
□ 避免在持锁时调用外部代码
□ 原子操作用于简单计数器
□ 无锁数据结构（高级）
```

---

**参考**: MISRA C:2023 Guidelines for the use of the C language in critical systems, Chapter 9

---

> **下一章节**: [Rules 10.1-10.8 - 运行时错误处理](./10_MISRA_C_2023_Rules_10.md)


---

## 深入理解

### 核心概念

本主题的核心概念包括：基础理论、实现机制、实际应用。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 学习建议

1. 先理解基础概念
2. 再进行实践练习
3. 最后深入源码

---

> **最后更新**: 2026-03-21  
> **维护者**: AI Code Review
