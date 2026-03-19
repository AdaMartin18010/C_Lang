# 协程与纤程 (Coroutines & Fibers)

## 目录

- [协程与纤程 (Coroutines \& Fibers)](#协程与纤程-coroutines--fibers)
  - [目录](#目录)
  - [协程概述](#协程概述)
    - [什么是协程](#什么是协程)
    - [协程原语](#协程原语)
  - [用户态上下文切换](#用户态上下文切换)
    - [使用ucontext (POSIX)](#使用ucontext-posix)
    - [汇编实现上下文切换](#汇编实现上下文切换)
  - [有栈协程实现](#有栈协程实现)
    - [完整实现](#完整实现)
  - [无栈协程实现](#无栈协程实现)
    - [状态机协程](#状态机协程)
    - [基于宏的异步/等待](#基于宏的异步等待)
  - [C++20协程移植](#c20协程移植)
    - [概念映射到C](#概念映射到c)
  - [应用模式](#应用模式)
    - [异步IO](#异步io)
    - [生成器模式](#生成器模式)
  - [参考](#参考)

---

## 协程概述

### 什么是协程

```
协程: 可以暂停和恢复执行的函数

对比:
- 函数: 调用->执行->返回 (一次性)
- 协程: 调用->执行->暂停->恢复->...->返回 (多次)

优势:
1. 异步代码同步写法
2. 避免回调地狱
3. 轻量级 (比线程轻量1000x)
4. 协作式调度

分类:
- 有栈协程 (Stackful): 每个协程独立栈，可任意挂起点
- 无栈协程 (Stackless): 状态机实现，特定挂起点
```

### 协程原语

```c
/* 协程基本操作 */

/* 创建协程 */
coroutine_t* co_create(void (*fn)(void*), void* arg, size_t stack_size);

/* 恢复执行 */
void co_resume(coroutine_t* co);

/* 挂起当前协程 */
void co_yield(void);

/* 切换到调度器 */
void co_return_to_scheduler(void);

/* 状态查询 */
co_status_t co_status(coroutine_t* co);
```

---

## 用户态上下文切换

### 使用ucontext (POSIX)

```c
#define _XOPEN_SOURCE 600
#include <ucontext.h>
#include <stdlib.h>
#include <stdio.h>

/* ucontext已被标记为废弃，但广泛用于教学 */

typedef struct {
    ucontext_t ctx;
    void (*fn)(void*);
    void* arg;
    void* stack;
    size_t stack_size;
    enum { CO_READY, CO_RUNNING, CO_SUSPENDED, CO_DEAD } status;
} coroutine_t;

static ucontext_t main_ctx;
static coroutine_t* current = NULL;

coroutine_t* co_create(void (*fn)(void*), void* arg, size_t stack_size) {
    coroutine_t* co = malloc(sizeof(coroutine_t));
    co->fn = fn;
    co->arg = arg;
    co->stack_size = stack_size ? stack_size : 64 * 1024;
    co->stack = malloc(co->stack_size);
    co->status = CO_READY;

    getcontext(&co->ctx);
    co->ctx.uc_stack.ss_sp = co->stack;
    co->ctx.uc_stack.ss_size = co->stack_size;
    co->ctx.uc_link = &main_ctx;

    makecontext(&co->ctx, (void (*)(void))co->fn, 1, co->arg);

    return co;
}

void co_resume(coroutine_t* co) {
    if (co->status == CO_DEAD) return;

    current = co;
    co->status = CO_RUNNING;
    swapcontext(&main_ctx, &co->ctx);
}

void co_yield(void) {
    if (current == NULL) return;

    current->status = CO_SUSPENDED;
    coroutine_t* co = current;
    current = NULL;
    swapcontext(&co->ctx, &main_ctx);
}

void co_exit(void) {
    if (current) {
        current->status = CO_DEAD;
        coroutine_t* co = current;
        current = NULL;
        swapcontext(&co->ctx, &main_ctx);
    }
}

/* 使用示例 */
void coroutine_func(void* arg) {
    int n = *(int*)arg;

    for (int i = 0; i < n; i++) {
        printf("Coroutine: %d\n", i);
        co_yield();  /* 挂起 */
    }

    co_exit();
}

void example(void) {
    int arg = 5;
    coroutine_t* co = co_create(coroutine_func, &arg, 0);

    /* 交替执行 */
    while (co->status != CO_DEAD) {
        printf("Main: before resume\n");
        co_resume(co);
        printf("Main: after yield\n");
    }

    free(co->stack);
    free(co);
}
```

### 汇编实现上下文切换

```c
/* x86_64 汇编上下文切换 - 更快 */

typedef struct {
    void* rsp;  /* 栈指针 */
    void* rbp;
    void* rip;  /* 指令指针 */
    void* rbx;
    void* r12;
    void* r13;
    void* r14;
    void* r15;
} context_t;

/* 保存当前上下文到from，恢复to上下文 */
__attribute__((naked)) void context_switch(context_t* from, context_t* to) {
    __asm__ volatile (
        /* 保存当前寄存器 */
        "movq %rsp, 0(%rdi)\n\t"
        "movq %rbp, 8(%rdi)\n\t"
        "movq (%rsp), %rax\n\t"  /* 返回地址 */
        "movq %rax, 16(%rdi)\n\t"
        "movq %rbx, 24(%rdi)\n\t"
        "movq %r12, 32(%rdi)\n\t"
        "movq %r13, 40(%rdi)\n\t"
        "movq %r14, 48(%rdi)\n\t"
        "movq %r15, 56(%rdi)\n\t"

        /* 恢复目标寄存器 */
        "movq 56(%rsi), %r15\n\t"
        "movq 48(%rsi), %r14\n\t"
        "movq 40(%rsi), %r13\n\t"
        "movq 32(%rsi), %r12\n\t"
        "movq 24(%rsi), %rbx\n\t"
        "movq 16(%rsi), %rax\n\t"
        "movq %rax, (%rsp)\n\t"   /* 恢复返回地址 */
        "movq 8(%rsi), %rbp\n\t"
        "movq 0(%rsi), %rsp\n\t"

        "ret\n\t"
    );
}
```

---

## 有栈协程实现

### 完整实现

```c
#define MAX_COROUTINES 1024
#define DEFAULT_STACK (64 * 1024)

typedef struct coroutine {
    int id;
    context_t ctx;
    void* stack;
    size_t stack_size;
    void (*fn)(void*);
    void* arg;
    enum { READY, RUNNING, SUSPENDED, DEAD } status;
    struct coroutine* next;
} coroutine;

static coroutine* coroutines[MAX_COROUTINES];
static coroutine* current_co = NULL;
static coroutine* ready_queue_head = NULL;
static coroutine* ready_queue_tail = NULL;
static int next_id = 0;
static context_t main_context;

/* 调度器 */
void schedule(void) {
    while (ready_queue_head != NULL) {
        /* 取出就绪协程 */
        coroutine* co = ready_queue_head;
        ready_queue_head = co->next;
        if (ready_queue_head == NULL) {
            ready_queue_tail = NULL;
        }

        if (co->status == DEAD) continue;

        /* 执行 */
        current_co = co;
        co->status = RUNNING;
        context_switch(&main_context, &co->ctx);

        /* 协程挂起或完成 */
        if (co->status != DEAD) {
            co->status = SUSPENDED;
        }
    }
    current_co = NULL;
}

/* 创建 */
int co_spawn(void (*fn)(void*), void* arg) {
    int id = next_id++;
    coroutine* co = malloc(sizeof(coroutine));

    co->id = id;
    co->fn = fn;
    co->arg = arg;
    co->status = READY;
    co->stack_size = DEFAULT_STACK;
    co->stack = malloc(co->stack_size);
    co->next = NULL;

    /* 初始化上下文 */
    co->ctx.rsp = (char*)co->stack + co->stack_size - sizeof(void*);
    co->ctx.rip = (void*)fn;
    co->ctx.rbp = NULL;

    coroutines[id] = co;

    /* 加入就绪队列 */
    if (ready_queue_tail) {
        ready_queue_tail->next = co;
    } else {
        ready_queue_head = co;
    }
    ready_queue_tail = co;

    return id;
}

/* 挂起 */
void co_yield(void) {
    if (current_co == NULL) return;

    coroutine* co = current_co;
    current_co = NULL;

    /* 保存当前上下文，切换到主上下文 */
    context_switch(&co->ctx, &main_context);
}

/* 恢复 */
void co_resume_id(int id) {
    coroutine* co = coroutines[id];
    if (co == NULL || co->status == DEAD) return;

    /* 加入就绪队列 */
    co->status = READY;
    if (ready_queue_tail) {
        ready_queue_tail->next = co;
    } else {
        ready_queue_head = co;
    }
    ready_queue_tail = co;
}

/* 完成 */
void co_done(void) {
    if (current_co) {
        current_co->status = DEAD;
        context_switch(&current_co->ctx, &main_context);
    }
}
```

---

## 无栈协程实现

### 状态机协程

```c
/* 使用switch实现的协程 (Duff's device) */

#define cr_begin static int __state = 0; switch(__state) { case 0:
#define cr_yield(x) do { __state = __LINE__; return x; case __LINE__:; } while(0)
#define cr_end } __state = 0
#define cr_reset __state = 0

typedef struct {
    int value;
    int __state;
} generator_t;

/* 生成器示例 */
int fibonacci(generator_t* g) {
    switch(g->__state) {
        case 0:
            g->value = 0;
            g->__state = 1;
            return g->value;

        case 1:
            g->value = 1;
            g->__state = 2;
            return g->value;

        case 2:
            while (1) {
                int next = g->value +
                    ((int*)g)[-1];  /* 需要保存前一个值 */
                ((int*)g)[-1] = g->value;
                g->value = next;
                g->__state = 3;
                return g->value;

        case 3:;
            }
    }
    return -1;
}

/* 使用 */
void use_generator(void) {
    generator_t gen = {0};

    for (int i = 0; i < 10; i++) {
        printf("%d ", fibonacci(&gen));
    }
    printf("\n");
}
```

### 基于宏的异步/等待

```c
/* 模拟async/await */

typedef struct {
    void* (*async_fn)(struct async_ctx*);
    void* result;
    int completed;
} async_ctx_t;

#define async_begin(ctx) \
    do { \
        static int __state = 0; \
        switch(__state) { \
            case 0:

#define await(expr) \
    do { \
        __state = __LINE__; \
        ctx->async_fn = (void*(*)(async_ctx_t*))resume_##__LINE__; \
        return (void*)1; \
        case __LINE__: \
            if (!(expr)) return (void*)1; \
    } while(0)

#define async_end \
        } \
        ctx->completed = 1; \
        return NULL; \
    } while(0)

/* 示例 */
void* fetch_data_async(async_ctx_t* ctx) {
    async_begin(ctx);

    await(io_pending(fd));      /* 等待IO就绪 */
    await(read_complete(fd));   /* 等待读取完成 */

    ctx->result = process_data();

    async_end;
}
```

---

## C++20协程移植

### 概念映射到C

```c
/* C++20协程概念:
 * - co_await: 等待可等待对象
 * - co_yield: 产生值并挂起
 * - co_return: 返回值并结束
 *
 * C实现: 使用函数指针和状态机
 */

typedef struct {
    void* promise;
    void* handle;
    int done;
} coro_handle_t;

typedef struct {
    coro_handle_t handle;
    void* value;
} awaitable_t;

/* co_await */
#define co_await(expr) ({ \
    awaitable_t a = (expr); \
    if (!a.handle.done) { \
        save_state(__LINE__); \
        suspend(); \
        case __LINE__:; \
    } \
    a.value; \
})

/* co_yield */
#define co_yield(value) do { \
    current_promise->value = (value); \
    save_state(__LINE__); \
    suspend(); \
    case __LINE__:; \
} while(0)

/* co_return */
#define co_return(value) do { \
    current_promise->return_value = (value); \
    current_coro->done = 1; \
    return; \
} while(0)
```

---

## 应用模式

### 异步IO

```c
/* 使用协程实现异步IO */

typedef struct {
    int fd;
    coroutine* co;
} aio_request_t;

static aio_request_t* pending_aio[1024];
static int epoll_fd;

/* 异步读取 */
void async_read(int fd, void* buf, size_t len) {
    /* 注册到epoll */
    struct epoll_event ev = {
        .events = EPOLLIN,
        .data.ptr = current_co
    };
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev);

    /* 挂起协程 */
    co_yield();

    /* 恢复后执行读取 */
    read(fd, buf, len);
}

/* IO调度循环 */
void io_scheduler(void) {
    struct epoll_event events[64];

    while (1) {
        int n = epoll_wait(epoll_fd, events, 64, 0);

        /* 恢复就绪的协程 */
        for (int i = 0; i < n; i++) {
            coroutine* co = events[i].data.ptr;
            co_resume(co);
        }

        /* 运行其他就绪协程 */
        schedule();
    }
}

/* 示例 */
void http_handler(void* arg) {
    int client_fd = *(int*)arg;
    char buf[4096];

    /* 同步写法，实际异步执行 */
    async_read(client_fd, buf, sizeof(buf));
    process_request(buf);
    async_write(client_fd, response, response_len);

    close(client_fd);
}
```

### 生成器模式

```c
/* 无限序列生成器 */

typedef struct {
    int (*next)(void*);
    void* state;
} generator_t;

/* 范围生成器 */
int range_next(void* state) {
    int* st = state;
    return (*st)++;
}

generator_t range(int start) {
    int* state = malloc(sizeof(int));
    *state = start;
    return (generator_t){range_next, state};
}

/* 过滤生成器 */
typedef struct {
    generator_t* source;
    int (*pred)(int);
} filter_state_t;

int filter_next(void* state) {
    filter_state_t* fs = state;
    while (1) {
        int val = fs->source->next(fs->source->state);
        if (fs->pred(val)) return val;
    }
}

generator_t filter(generator_t* src, int (*p)(int)) {
    filter_state_t* st = malloc(sizeof(filter_state_t));
    st->source = src;
    st->pred = p;
    return (generator_t){filter_next, st};
}

/* 使用 */
void example_generator(void) {
    generator_t nums = range(1);
    generator_t evens = filter(&nums, is_even);

    for (int i = 0; i < 10; i++) {
        printf("%d ", evens.next(evens.state));
    }
}
```

---

## 参考

- [Protothreads](http://dunkels.com/adam/pt/)
- [libco](https://github.com/canonical/libco)
- [libdill](http://libdill.org/)
- [C++ Coroutines](https://en.cppreference.com/w/cpp/language/coroutines)


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
