# CSP 模型 C语言实现

## 目录

- [CSP 模型 C语言实现](#csp-模型-c语言实现)
  - [目录](#目录)
  - [CSP概述](#csp概述)
    - [什么是CSP](#什么是csp)
    - [CSP基本原语](#csp基本原语)
  - [Channel实现](#channel实现)
    - [有缓冲Channel](#有缓冲channel)
    - [无缓冲Channel](#无缓冲channel)
  - [Select多路复用](#select多路复用)
    - [Select实现](#select实现)
  - [Goroutine风格调度](#goroutine风格调度)
    - [轻量级线程](#轻量级线程)
  - [同步与异步](#同步与异步)
    - [同步转异步](#同步转异步)
  - [超时与取消](#超时与取消)
    - [Context模式](#context模式)
  - [管道模式](#管道模式)
    - [Fan-out/Fan-in](#fan-outfan-in)
    - [Pipeline](#pipeline)
  - [完整示例](#完整示例)
    - [并发素数筛](#并发素数筛)
    - [HTTP服务器示例](#http服务器示例)
  - [总结](#总结)
  - [参考资源](#参考资源)

---

## CSP概述

### 什么是CSP

```
CSP (Communicating Sequential Processes) 由Tony Hoare于1978年提出。

核心理念:
- 通过通信来共享内存，而不是通过共享内存来通信
- Channel是同步的、类型化的通信原语
- 发送和接收都可能阻塞

对比Actor模型:
- Actor: 异步消息，消息发送到邮箱
- CSP: 同步通信，发送者等待接收者就绪

成功应用:
- Go语言: goroutine + channel
- Occam: 并行编程语言
- Clojure core.async
- Python trio/anyio
```

### CSP基本原语

```c
/* CSP的核心操作 */

/* Channel创建 */
chan_t* ch = make_chan(sizeof(int), buffer_size);

/* 发送 (可能阻塞) */
chan_send(ch, &value);

/* 接收 (可能阻塞) */
chan_recv(ch, &value);

/* 关闭 */
chan_close(ch);

/* Select (多路复用) */
select_t cases[] = {
    {ch1, &v1, SELECT_RECV},
    {ch2, &v2, SELECT_SEND},
    {NULL, NULL, SELECT_DEFAULT}  /* 非阻塞 */
};
int chosen = chan_select(cases, 3);
```

---

## Channel实现

### 有缓冲Channel

```c
#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>

typedef struct {
    void* data;         /* 环形缓冲区 */
    size_t elem_size;   /* 元素大小 */
    size_t capacity;    /* 容量 */

    _Atomic(size_t) send_idx;   /* 发送索引 */
    _Atomic(size_t) recv_idx;   /* 接收索引 */

    mtx_t lock;
    cnd_t send_ready;   /* 发送者等待 */
    cnd_t recv_ready;   /* 接收者等待 */

    atomic_bool closed; /* 关闭标志 */
    atomic_int waiters; /* 等待者计数 */
} chan_t;

/* 创建Channel */
chan_t* make_chan(size_t elem_size, size_t capacity) {
    chan_t* ch = calloc(1, sizeof(chan_t));
    ch->elem_size = elem_size;
    ch->capacity = capacity;
    ch->data = calloc(capacity + 1, elem_size);  /* +1用于区分满/空 */

    atomic_init(&ch->send_idx, 0);
    atomic_init(&ch->recv_idx, 0);
    atomic_init(&ch->closed, false);
    atomic_init(&ch->waiters, 0);

    mtx_init(&ch->lock, mtx_plain);
    cnd_init(&ch->send_ready);
    cnd_init(&ch->recv_ready);

    return ch;
}

/* 发送 */
bool chan_send(chan_t* ch, void* elem) {
    if (atomic_load_explicit(&ch->closed, memory_order_acquire)) {
        return false;  /* Channel已关闭 */
    }

    mtx_lock(&ch->lock);

    while (1) {
        size_t send_idx = atomic_load_explicit(&ch->send_idx,
                                                memory_order_relaxed);
        size_t recv_idx = atomic_load_explicit(&ch->recv_idx,
                                                memory_order_relaxed);
        size_t count = (send_idx - recv_idx + ch->capacity + 1)
                       % (ch->capacity + 1);

        if (count < ch->capacity) {
            /* 有空间，直接写入 */
            char* slot = (char*)ch->data + send_idx * ch->elem_size;
            memcpy(slot, elem, ch->elem_size);
            atomic_store_explicit(&ch->send_idx,
                                  (send_idx + 1) % (ch->capacity + 1),
                                  memory_order_release);

            cnd_signal(&ch->recv_ready);  /* 唤醒接收者 */
            mtx_unlock(&ch->lock);
            return true;
        }

        /* 满了，等待 */
        if (atomic_load_explicit(&ch->closed, memory_order_relaxed)) {
            mtx_unlock(&ch->lock);
            return false;
        }

        atomic_fetch_add(&ch->waiters, 1);
        cnd_wait(&ch->send_ready, &ch->lock);
        atomic_fetch_sub(&ch->waiters, 1);
    }
}

/* 接收 */
bool chan_recv(chan_t* ch, void* elem) {
    mtx_lock(&ch->lock);

    while (1) {
        size_t send_idx = atomic_load_explicit(&ch->send_idx,
                                                memory_order_acquire);
        size_t recv_idx = atomic_load_explicit(&ch->recv_idx,
                                                memory_order_relaxed);

        if (send_idx != recv_idx) {
            /* 有数据 */
            char* slot = (char*)ch->data + recv_idx * ch->elem_size;
            memcpy(elem, slot, ch->elem_size);
            atomic_store_explicit(&ch->recv_idx,
                                  (recv_idx + 1) % (ch->capacity + 1),
                                  memory_order_release);

            cnd_signal(&ch->send_ready);  /* 唤醒发送者 */
            mtx_unlock(&ch->lock);
            return true;
        }

        /* 空，检查是否关闭 */
        if (atomic_load_explicit(&ch->closed, memory_order_acquire)) {
            mtx_unlock(&ch->lock);
            return false;  /* Channel关闭且无数据 */
        }

        /* 等待 */
        atomic_fetch_add(&ch->waiters, 1);
        cnd_wait(&ch->recv_ready, &ch->lock);
        atomic_fetch_sub(&ch->waiters, 1);
    }
}

/* 关闭Channel */
void chan_close(chan_t* ch) {
    mtx_lock(&ch->lock);
    atomic_store_explicit(&ch->closed, true, memory_order_release);

    /* 唤醒所有等待者 */
    cnd_broadcast(&ch->send_ready);
    cnd_broadcast(&ch->recv_ready);
    mtx_unlock(&ch->lock);
}

/* 清理 */
void chan_free(chan_t* ch) {
    mtx_destroy(&ch->lock);
    cnd_destroy(&ch->send_ready);
    cnd_destroy(&ch->recv_ready);
    free(ch->data);
    free(ch);
}
```

### 无缓冲Channel

```c
/* 无缓冲Channel - 同步点 */

typedef struct {
    void* sender_data;   /* 发送者数据 */
    void* recv_buffer;   /* 接收者缓冲区 */
    atomic_bool ready;   /* 就绪标志 */
    mtx_t lock;
    cnd_t cond;
} unbuf_chan_t;

unbuf_chan_t* make_unbuf_chan(void) {
    unbuf_chan_t* ch = calloc(1, sizeof(unbuf_chan_t));
    atomic_init(&ch->ready, false);
    mtx_init(&ch->lock, mtx_plain);
    cnd_init(&ch->cond);
    return ch;
}

/* 发送 - 阻塞直到有接收者 */
bool unbuf_send(unbuf_chan_t* ch, void* data, size_t size) {
    mtx_lock(&ch->lock);

    /* 等待接收者 */
    while (!atomic_load_explicit(&ch->ready, memory_order_acquire)) {
        cnd_wait(&ch->cond, &ch->lock);
    }

    /* 直接复制到接收者缓冲区 */
    memcpy(ch->recv_buffer, data, size);
    atomic_store_explicit(&ch->ready, false, memory_order_release);
    cnd_signal(&ch->cond);

    mtx_unlock(&ch->lock);
    return true;
}

/* 接收 - 阻塞直到有发送者 */
bool unbuf_recv(unbuf_chan_t* ch, void* buffer, size_t size) {
    mtx_lock(&ch->lock);

    /* 通知发送者我们准备好了 */
    ch->recv_buffer = buffer;
    atomic_store_explicit(&ch->ready, true, memory_order_release);
    cnd_signal(&ch->cond);

    /* 等待发送者完成 */
    while (atomic_load_explicit(&ch->ready, memory_order_acquire)) {
        cnd_wait(&ch->cond, &ch->lock);
    }

    mtx_unlock(&ch->lock);
    return true;
}
```

---

## Select多路复用

### Select实现

```c
typedef enum {
    SELECT_SEND,
    SELECT_RECV,
    SELECT_DEFAULT
} select_op_t;

typedef struct {
    chan_t* ch;
    void* ptr;          /* 发送/接收数据指针 */
    select_op_t op;
    bool selected;      /* 是否被选中 */
} select_case_t;

/* 非阻塞尝试 */
static bool try_op(select_case_t* c) {
    if (c->op == SELECT_SEND) {
        /* 尝试发送 */
        mtx_lock(&c->ch->lock);
        size_t send_idx = atomic_load(&c->ch->send_idx);
        size_t recv_idx = atomic_load(&c->ch->recv_idx);
        size_t count = (send_idx - recv_idx + c->ch->capacity + 1)
                       % (c->ch->capacity + 1);

        if (count < c->ch->capacity) {
            /* 可以发送 */
            char* slot = (char*)c->ch->data + send_idx * c->ch->elem_size;
            memcpy(slot, c->ptr, c->ch->elem_size);
            atomic_store(&c->ch->send_idx,
                        (send_idx + 1) % (c->ch->capacity + 1));
            cnd_signal(&c->ch->recv_ready);
            mtx_unlock(&c->ch->lock);
            return true;
        }
        mtx_unlock(&c->ch->lock);
        return false;

    } else if (c->op == SELECT_RECV) {
        /* 尝试接收 */
        mtx_lock(&c->ch->lock);
        size_t send_idx = atomic_load(&c->ch->send_idx);
        size_t recv_idx = atomic_load(&c->ch->recv_idx);

        if (send_idx != recv_idx) {
            /* 有数据 */
            char* slot = (char*)c->ch->data + recv_idx * c->ch->elem_size;
            memcpy(c->ptr, slot, c->ch->elem_size);
            atomic_store(&c->ch->recv_idx,
                        (recv_idx + 1) % (c->ch->capacity + 1));
            cnd_signal(&c->ch->send_ready);
            mtx_unlock(&c->ch->lock);
            return true;
        }
        mtx_unlock(&c->ch->lock);
        return false;
    }
    return true;  /* DEFAULT */
}

/* Select实现 */
int chan_select(select_case_t* cases, int n, int timeout_ms) {
    /* 1. 随机打乱顺序，避免饥饿 */
    int order[n];
    for (int i = 0; i < n; i++) order[i] = i;

    /* Fisher-Yates shuffle */
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = order[i];
        order[i] = order[j];
        order[j] = tmp;
    }

    /* 2. 先尝试非阻塞 */
    for (int i = 0; i < n; i++) {
        int idx = order[i];
        if (cases[idx].op == SELECT_DEFAULT) {
            return idx;  /* 立即返回 */
        }
        if (try_op(&cases[idx])) {
            return idx;
        }
    }

    /* 3. 需要阻塞等待 */
    mtx_t poll_lock;
    cnd_t poll_cond;
    mtx_init(&poll_lock, mtx_plain);
    cnd_init(&poll_cond);

    /* 注册到所有channel的等待列表 */
    for (int i = 0; i < n; i++) {
        cases[i].selected = false;
        /* 实际实现需要维护每个channel的等待者列表 */
    }

    /* 等待直到某个channel就绪 */
    mtx_lock(&poll_lock);
    struct timespec deadline;
    timespec_get(&deadline, TIME_UTC);
    deadline.tv_sec += timeout_ms / 1000;
    deadline.tv_nsec += (timeout_ms % 1000) * 1000000;

    while (1) {
        /* 检查是否被唤醒 */
        for (int i = 0; i < n; i++) {
            if (cases[i].selected) {
                mtx_unlock(&poll_lock);
                return i;
            }
        }

        /* 超时检查 */
        if (timeout_ms >= 0) {
            if (cnd_timedwait(&poll_cond, &poll_lock, &deadline)
                == thrd_timeout) {
                mtx_unlock(&poll_lock);
                return -1;
            }
        } else {
            cnd_wait(&poll_cond, &poll_lock);
        }
    }
}
```

---

## Goroutine风格调度

### 轻量级线程

```c
#include <threads.h>
#include <setjmp.h>

#define STACK_SIZE (256 * 1024)  /* 256KB栈 */
#define MAX_GOROUTINES 10000

typedef enum {
    G_RUNNING,
    G_RUNNABLE,
    G_WAITING,      /* 在channel上等待 */
    G_DEAD
} g_status_t;

typedef struct g {
    int id;
    g_status_t status;
    jmp_buf context;
    void* stack;
    size_t stack_size;
    struct g* next;  /* 运行队列链表 */
} g_t;

/* 全局调度器 */
typedef struct {
    g_t* run_queue_head;
    g_t* run_queue_tail;
    mtx_t lock;
    cnd_t cond;
    g_t* current;
} scheduler_t;

static scheduler_t g_scheduler;

/* 创建goroutine */
g_t* go(void (*fn)(void*), void* arg) {
    g_t* g = calloc(1, sizeof(g_t));
    g->id = atomic_fetch_add(&next_goid, 1);
    g->status = G_RUNNABLE;
    g->stack = malloc(STACK_SIZE);
    g->stack_size = STACK_SIZE;

    /* 初始化栈 */
    void** sp = (void**)(g->stack + STACK_SIZE - sizeof(void*) * 2);
    sp[0] = arg;      /* 参数 */
    sp[1] = g_exit;   /* 返回地址 */

    /* 设置上下文 */
    setjmp(g->context);
    /* 修改jmp_buf设置栈指针 */
    /* 平台相关代码 */

    /* 加入运行队列 */
    mtx_lock(&g_scheduler.lock);
    if (g_scheduler.run_queue_tail) {
        g_scheduler.run_queue_tail->next = g;
    } else {
        g_scheduler.run_queue_head = g;
    }
    g_scheduler.run_queue_tail = g;
    cnd_signal(&g_scheduler.cond);
    mtx_unlock(&g_scheduler.lock);

    return g;
}

/* 调度循环 */
static int scheduler_thread(void* arg) {
    while (1) {
        mtx_lock(&g_scheduler.lock);

        /* 获取可运行的goroutine */
        while (g_scheduler.run_queue_head == NULL) {
            cnd_wait(&g_scheduler.cond, &g_scheduler.lock);
        }

        g_t* g = g_scheduler.run_queue_head;
        g_scheduler.run_queue_head = g->next;
        if (g_scheduler.run_queue_head == NULL) {
            g_scheduler.run_queue_tail = NULL;
        }

        g->status = G_RUNNING;
        g_scheduler.current = g;
        mtx_unlock(&g_scheduler.lock);

        /* 切换到goroutine */
        longjmp(g->context, 1);

        /* goroutine yield或完成会回到这里 */
    }
    return 0;
}

/* Yield让出CPU */
void g_yield(void) {
    g_t* g = g_scheduler.current;

    if (setjmp(g->context) == 0) {
        /* 保存状态，回到调度器 */
        /* 实际上需要更复杂的上下文切换 */

        /* 重新加入队列 */
        mtx_lock(&g_scheduler.lock);
        g->status = G_RUNNABLE;
        if (g_scheduler.run_queue_tail) {
            g_scheduler.run_queue_tail->next = g;
        } else {
            g_scheduler.run_queue_head = g;
        }
        g_scheduler.run_queue_tail = g;
        mtx_unlock(&g_scheduler.lock);

        /* 切换到调度器 */
    }
}

/* Goroutine退出 */
void g_exit(void) {
    g_t* g = g_scheduler.current;
    g->status = G_DEAD;

    /* 清理资源 */
    free(g->stack);

    /* 永远不会返回 */
    while (1) {
        g_yield();
    }
}
```

---

## 同步与异步

### 同步转异步

```c
/* Future/Promise模式 */

typedef struct {
    void* value;
    atomic_bool ready;
    mtx_t lock;
    cnd_t cond;
    chan_t* waiters;  /* 等待结果的channel */
} future_t;

typedef struct {
    future_t* future;
} promise_t;

future_t* make_future(void) {
    future_t* f = calloc(1, sizeof(future_t));
    atomic_init(&f->ready, false);
    mtx_init(&f->lock, mtx_plain);
    cnd_init(&f->cond);
    return f;
}

promise_t make_promise(future_t* f) {
    return (promise_t){.future = f};
}

/* 设置结果 */
void promise_set(promise_t* p, void* value) {
    future_t* f = p->future;
    mtx_lock(&f->lock);
    f->value = value;
    atomic_store_explicit(&f->ready, true, memory_order_release);
    cnd_broadcast(&f->cond);
    mtx_unlock(&f->lock);
}

/* 获取结果 (阻塞) */
void* future_get(future_t* f) {
    mtx_lock(&f->lock);
    while (!atomic_load_explicit(&f->ready, memory_order_acquire)) {
        cnd_wait(&f->cond, &f->lock);
    }
    void* v = f->value;
    mtx_unlock(&f->lock);
    return v;
}

/* 使用 */
void async_task(void* arg) {
    future_t* f = arg;
    promise_t p = make_promise(f);

    /* 执行异步操作 */
    void* result = do_work();
    promise_set(&p, result);
}

void example(void) {
    future_t* f = make_future();
    go(async_task, f);

    /* 阻塞等待结果 */
    void* result = future_get(f);
}
```

---

## 超时与取消

### Context模式

```c
/* Go风格的Context */

typedef struct context {
    atomic_bool cancelled;
    chan_t* done;       /* 关闭时通知 */
    int64_t deadline;   /* 截止时间 */
    struct context* parent;
} context_t;

context_t* background(void) {
    static context_t bg = {0};
    return &bg;
}

context_t* with_cancel(context_t* parent) {
    context_t* ctx = calloc(1, sizeof(context_t));
    ctx->parent = parent;
    ctx->done = make_chan(sizeof(int), 0);
    return ctx;
}

context_t* with_timeout(context_t* parent, int ms) {
    context_t* ctx = with_cancel(parent);
    ctx->deadline = current_time_ms() + ms;

    /* 启动定时器goroutine */
    go((void(*)(void*))timeout_cancel, ctx);

    return ctx;
}

void cancel(context_t* ctx) {
    if (atomic_compare_exchange_strong(&ctx->cancelled,
                                       &(bool){false}, true)) {
        chan_close(ctx->done);

        /* 取消子context */
        /* 实际实现需要维护子context列表 */
    }
}

/* 检查是否取消 */
bool ctx_done(context_t* ctx) {
    return atomic_load(&ctx->cancelled);
}

/* 使用 */
void worker(context_t* ctx, chan_t* input, chan_t* output) {
    while (!ctx_done(ctx)) {
        select_case_t cases[] = {
            {input, &data, SELECT_RECV},
            {ctx->done, NULL, SELECT_RECV}
        };
        int chosen = chan_select(cases, 2, -1);

        if (chosen == 1) {
            return;  /* 取消 */
        }

        /* 处理数据 */
        process(data);
    }
}
```

---

## 管道模式

### Fan-out/Fan-in

```c
/* Fan-out: 一个输入，多个处理 */
void fan_out(chan_t* input, chan_t** outputs, int n) {
    for (int i = 0; i < n; i++) {
        go((void(*)(void*))fan_out_worker,
           &(fan_out_args){input, outputs[i]});
    }
}

void fan_out_worker(fan_out_args* args) {
    int value;
    while (chan_recv(args->input, &value)) {
        chan_send(args->output, &value);
    }
    chan_close(args->output);
}

/* Fan-in: 多个输入，一个输出 */
chan_t* fan_in(chan_t** inputs, int n) {
    chan_t* output = make_chan(sizeof(int), 0);

    for (int i = 0; i < n; i++) {
        go((void(*)(void*))fan_in_worker,
           &(fan_in_args){inputs[i], output});
    }

    /* 等待所有输入关闭后关闭输出 */
    go((void(*)(void*))fan_in_closer,
       &(fan_in_close_args){inputs, n, output});

    return output;
}

void fan_in_worker(fan_in_args* args) {
    int value;
    while (chan_recv(args->input, &value)) {
        chan_send(args->output, &value);
    }
}
```

### Pipeline

```c
/* 流水线阶段 */
typedef chan_t* (*stage_fn)(chan_t*);

chan_t* pipeline(chan_t* input, stage_fn* stages, int n) {
    chan_t* current = input;

    for (int i = 0; i < n; i++) {
        current = stages[i](current);
    }

    return current;
}

/* 示例阶段 */
chan_t* stage_filter(chan_t* input) {
    chan_t* output = make_chan(sizeof(int), 10);

    go((void(*)(void*))filter_worker, &(filter_args){input, output});

    return output;
}

void filter_worker(filter_args* args) {
    int value;
    while (chan_recv(args->input, &value)) {
        if (value % 2 == 0) {  /* 过滤偶数 */
            chan_send(args->output, &value);
        }
    }
    chan_close(args->output);
}

/* 使用 */
void example_pipeline(void) {
    chan_t* input = make_chan(sizeof(int), 10);

    stage_fn stages[] = {
        stage_filter,
        stage_square,
        stage_sum
    };

    chan_t* output = pipeline(input, stages, 3);

    /* 发送数据 */
    for (int i = 1; i <= 100; i++) {
        chan_send(input, &i);
    }
    chan_close(input);

    /* 接收结果 */
    int result;
    chan_recv(output, &result);
    printf("Sum: %d\n", result);
}
```

---

## 完整示例

### 并发素数筛

```c
/* 埃拉托斯特尼筛法的并发实现 */

void filter(chan_t* in, chan_t* out, int prime) {
    int n;
    while (chan_recv(in, &n)) {
        if (n % prime != 0) {
            chan_send(out, &n);
        }
    }
    chan_close(out);
}

void sieve(void) {
    chan_t* ch = make_chan(sizeof(int), 100);

    /* 生成数字 */
    go((void(*)(void*))generate, ch);

    /* 筛法管道 */
    for (int i = 0; i < 10; i++) {
        int prime;
        if (!chan_recv(ch, &prime)) {
            break;
        }
        printf("%d ", prime);

        chan_t* next = make_chan(sizeof(int), 100);
        go((void(*)(void*))filter, &(filter_args){ch, next, prime});
        ch = next;
    }
    printf("\n");
}

void generate(chan_t* ch) {
    for (int i = 2; ; i++) {
        chan_send(ch, &i);
    }
}
```

### HTTP服务器示例

```c
/* 使用CSP模型实现的并发HTTP服务器 */

typedef struct {
    int fd;
    char client_ip[16];
} connection_t;

void http_server(int port) {
    int listen_fd = socket_listen(port);
    chan_t* conn_ch = make_chan(sizeof(connection_t), 100);

    /* 启动工作池 */
    for (int i = 0; i < 10; i++) {
        go((void(*)(void*))worker, conn_ch);
    }

    /* 接受连接 */
    while (1) {
        connection_t conn;
        conn.fd = accept(listen_fd, conn.client_ip);

        select_case_t cases[] = {
            {conn_ch, &conn, SELECT_SEND},
            {NULL, NULL, SELECT_DEFAULT}
        };

        if (chan_select(cases, 2, 0) == 1) {
            /* 队列满，拒绝连接 */
            close(conn.fd);
        }
    }
}

void worker(chan_t* conn_ch) {
    connection_t conn;
    while (chan_recv(conn_ch, &conn)) {
        handle_request(conn.fd);
        close(conn.fd);
    }
}
```

---

## 总结

CSP模型的关键要点：

1. **Channel是核心**: 类型化的同步通信原语
2. **Select多路复用**: 同时等待多个channel
3. **Goroutine调度**: 轻量级线程，高效并发
4. **组合性**: 管道、Fan-out/Fan-in等模式

Go语言证明了CSP模型在系统编程中的有效性，C语言可以实现相同的核心原语。

---

## 参考资源

- [Communicating Sequential Processes (CSP)](https://www.cs.cmu.edu/~crary/819-f09/Hoare78.pdf)
- [Go Concurrency Patterns](https://go.dev/blog/pipelines)
- [Advanced Go Concurrency Patterns](https://go.dev/blog/io2013-talk-concurrency)
- [CSP for Python](https://trio.readthedocs.io/)
