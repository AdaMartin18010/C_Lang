---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../01_Core_Knowledge_System/05_Engineering_Layer/readme.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../02_Formal_Semantics_and_Physics/readme.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../03_System_Technology_Domains/readme.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../04_Industrial_Scenarios/readme.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../06_Thinking_Representation/readme.md) | 核心关联 | 思维表征基础 |
﻿# 事件驱动架构 (EDA)

## 目录

- [事件驱动架构 (EDA)](#事件驱动架构-eda)
  - [目录](#目录)
  - [Reactor模式](#reactor模式)
    - [核心概念](#核心概念)
    - [C实现](#c实现)
  - [Proactor模式](#proactor模式)
  - [事件总线](#事件总线)
  - [Saga模式](#saga模式)
  - [CQRS模式](#cqrs模式)
  - [参考](#参考)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)

---

## Reactor模式

### 核心概念

```
Reactor: 同步事件多路分解和分发

组件:
- Reactor: 核心，注册和分发事件
- Handler: 事件处理器
- Synchronous Event Demultiplexer: select/poll/epoll

流程:
1. 注册Handler到Reactor
2. Reactor等待事件 (阻塞在demultiplexer)
3. 事件到达，Reactor分发到对应Handler
4. Handler处理事件 (同步)

案例: Redis, Nginx, Netty
```

### C实现

```c
#include <sys/epoll.h>
#include <stdlib.h>
#include <string.h>

#define MAX_EVENTS 1024
#define MAX_HANDLERS 65536

/* 事件类型 */
typedef enum {
    EVENT_READ  = EPOLLIN,
    EVENT_WRITE = EPOLLOUT,
    EVENT_ERROR = EPOLLERR,
} event_type_t;

/* 事件处理器接口 */
typedef struct handler {
    int fd;
    void (*handle_read)(struct handler*);
    void (*handle_write)(struct handler*);
    void (*handle_error)(struct handler*);
    void* context;
} handler_t;

/* Reactor */
typedef struct {
    int epoll_fd;
    handler_t* handlers[MAX_HANDLERS];  /* fd -> handler映射 */
    int running;
} reactor_t;

reactor_t* reactor_create(void) {
    reactor_t* r = calloc(1, sizeof(reactor_t));
    r->epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    return r;
}

/* 注册处理器 */
int reactor_register(reactor_t* r, int fd, handler_t* handler,
                     uint32_t events) {
    struct epoll_event ev = {
        .events = events,
        .data.ptr = handler
    };

    if (epoll_ctl(r->epoll_fd, EPOLL_CTL_ADD, fd, &ev) < 0) {
        return -1;
    }

    r->handlers[fd] = handler;
    handler->fd = fd;
    return 0;
}

/* 修改事件 */
int reactor_modify(reactor_t* r, int fd, uint32_t events) {
    struct epoll_event ev = {
        .events = events,
        .data.ptr = r->handlers[fd]
    };
    return epoll_ctl(r->epoll_fd, EPOLL_CTL_MOD, fd, &ev);
}

/* 注销 */
void reactor_unregister(reactor_t* r, int fd) {
    epoll_ctl(r->epoll_fd, EPOLL_CTL_DEL, fd, NULL);
    r->handlers[fd] = NULL;
}

/* 事件循环 */
void reactor_run(reactor_t* r) {
    struct epoll_event events[MAX_EVENTS];
    r->running = 1;

    while (r->running) {
        int n = epoll_wait(r->epoll_fd, events, MAX_EVENTS, -1);

        for (int i = 0; i < n; i++) {
            handler_t* h = events[i].data.ptr;
            uint32_t ev = events[i].events;

            if ((ev & EPOLLERR) || (ev & EPOLLHUP)) {
                if (h->handle_error) h->handle_error(h);
                continue;
            }

            if (ev & EPOLLIN) {
                if (h->handle_read) h->handle_read(h);
            }

            if (ev & EPOLLOUT) {
                if (h->handle_write) h->handle_write(h);
            }
        }
    }
}

void reactor_stop(reactor_t* r) {
    r->running = 0;
}

/* TCP服务器Handler示例 */
typedef struct {
    handler_t base;
    char buffer[4096];
    int buf_len;
} tcp_conn_handler_t;

void tcp_handle_read(handler_t* h) {
    tcp_conn_handler_t* conn = (tcp_conn_handler_t*)h;

    ssize_t n = read(h->fd, conn->buffer + conn->buf_len,
                     sizeof(conn->buffer) - conn->buf_len);

    if (n > 0) {
        conn->buf_len += n;
        /* 处理数据... */
        process_data(conn->buffer, conn->buf_len);
    } else if (n == 0) {
        /* 连接关闭 */
        close(h->fd);
        free(conn);
    } else {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            perror("read");
        }
    }
}

void tcp_handle_write(handler_t* h) {
    /* 发送数据... */
}

/* 使用 */
void example_reactor_server(void) {
    reactor_t* reactor = reactor_create();

    int listen_fd = socket_listen(8080);

    /* 接受连接Handler */
    handler_t accept_handler = {
        .fd = listen_fd,
        .handle_read = [](handler_t* h) {
            int conn_fd = accept(h->fd, NULL, NULL);
            set_nonblocking(conn_fd);

            tcp_conn_handler_t* conn = calloc(1, sizeof(tcp_conn_handler_t));
            conn->base.handle_read = tcp_handle_read;
            conn->base.handle_write = tcp_handle_write;

            reactor_register(reactor, conn_fd, &conn->base, EPOLLIN | EPOLLET);
        }
    };

    reactor_register(reactor, listen_fd, &accept_handler, EPOLLIN);
    reactor_run(reactor);
}
```

---

## Proactor模式

```c
/* Proactor: 异步完成处理 */

#include <aio.h>
#include <signal.h>

typedef struct {
    void (*complete)(struct aiocb* cb, ssize_t nbytes);
    void* context;
} aio_callback_t;

/* 异步读取 */
void async_read(int fd, void* buf, size_t len, off_t offset,
                void (*callback)(struct aiocb*, ssize_t)) {
    struct aiocb* cb = calloc(1, sizeof(struct aiocb));
    cb->aio_fildes = fd;
    cb->aio_buf = buf;
    cb->aio_nbytes = len;
    cb->aio_offset = offset;

    /* 设置完成通知 */
    cb->aio_sigevent.sigev_notify = SIGEV_THREAD;
    cb->aio_sigevent.sigev_notify_function =
        (void (*)(sigval_t))callback;
    cb->aio_sigevent.sigev_value.sival_ptr = cb;

    aio_read(cb);
}

/* 使用io_uring的Proactor (Linux 5.1+) */
#include <liburing.h>

struct io_uring ring;

typedef void (*io_completion_t)(struct io_uring_cqe* cqe);

void proactor_init(void) {
    io_uring_queue_init(4096, &ring, 0);
}

void proactor_submit_read(int fd, void* buf, size_t len, off_t offset,
                          io_completion_t callback) {
    struct io_uring_sqe* sqe = io_uring_get_sqe(&ring);
    io_uring_prep_read(sqe, fd, buf, len, offset);
    io_uring_sqe_set_data(sqe, callback);
    io_uring_submit(&ring);
}

void proactor_run(void) {
    struct io_uring_cqe* cqe;

    while (1) {
        int ret = io_uring_wait_cqe(&ring, &cqe);
        if (ret < 0) continue;

        io_completion_t callback = io_uring_cqe_get_data(cqe);
        if (callback) {
            callback(cqe);
        }

        io_uring_cqe_seen(&ring, cqe);
    }
}
```

---

## 事件总线

```c
/* 类型安全的事件总线 */

typedef void (*event_handler_t)(void* event_data, void* context);

typedef struct event_subscription {
    int event_type;
    event_handler_t handler;
    void* context;
    struct event_subscription* next;
} event_subscription_t;

typedef struct {
    event_subscription_t* subs;
    mtx_t lock;
    /* 事件队列 */
    void** event_queue;
    int queue_head;
    int queue_tail;
    int queue_size;
    cnd_t has_event;
} event_bus_t;

event_bus_t* event_bus_create(int queue_size) {
    event_bus_t* bus = calloc(1, sizeof(event_bus_t));
    mtx_init(&bus->lock, mtx_plain);
    cnd_init(&bus->has_event);
    bus->event_queue = calloc(queue_size, sizeof(void*));
    bus->queue_size = queue_size;
    return bus;
}

/* 订阅 */
void event_bus_subscribe(event_bus_t* bus, int event_type,
                         event_handler_t handler, void* context) {
    event_subscription_t* sub = malloc(sizeof(event_subscription_t));
    sub->event_type = event_type;
    sub->handler = handler;
    sub->context = context;

    mtx_lock(&bus->lock);
    sub->next = bus->subs;
    bus->subs = sub;
    mtx_unlock(&bus->lock);
}

/* 发布 */
void event_bus_publish(event_bus_t* bus, int event_type, void* event_data) {
    mtx_lock(&bus->lock);

    /* 查找订阅者并直接调用 (同步) */
    event_subscription_t* sub = bus->subs;
    while (sub) {
        if (sub->event_type == event_type) {
            sub->handler(event_data, sub->context);
        }
        sub = sub->next;
    }

    mtx_unlock(&bus->lock);
}

/* 异步发布 */
void event_bus_publish_async(event_bus_t* bus, int event_type, void* event_data) {
    mtx_lock(&bus->lock);

    int next = (bus->queue_tail + 1) % bus->queue_size;
    if (next != bus->queue_head) {
        bus->event_queue[bus->queue_tail] = event_data;
        bus->queue_tail = next;
        cnd_signal(&bus->has_event);
    }

    mtx_unlock(&bus->lock);
}

/* 分发线程 */
int event_dispatch_thread(void* arg) {
    event_bus_t* bus = arg;

    while (1) {
        mtx_lock(&bus->lock);
        while (bus->queue_head == bus->queue_tail) {
            cnd_wait(&bus->has_event, &bus->lock);
        }

        void* event = bus->event_queue[bus->queue_head];
        bus->queue_head = (bus->queue_head + 1) % bus->queue_size;
        mtx_unlock(&bus->lock);

        /* 分发 */
        /* ... */
    }
    return 0;
}
```

---

## Saga模式

```c
/* Saga: 分布式事务协调 */

typedef enum {
    SAGA_STEP_PENDING,
    SAGA_STEP_SUCCEEDED,
    SAGA_STEP_FAILED,
    SAGA_STEP_COMPENSATED
} saga_step_status_t;

typedef struct saga_step {
    int id;
    char name[64];

    /* 执行动作 */
    int (*action)(void* context, void** result);

    /* 补偿动作 */
    int (*compensate)(void* context);

    saga_step_status_t status;
    void* result;
    void* context;
} saga_step_t;

typedef struct {
    saga_step_t* steps;
    int step_count;
    int current_step;

    void (*on_success)(void* result);
    void (*on_failure)(int failed_step, void* error);
} saga_t;

/* 编排式Saga (Orchestration) */
typedef struct {
    saga_t* saga;
    void* saga_context;
} saga_orchestrator_t;

void saga_execute(saga_orchestrator_t* orch) {
    saga_t* s = orch->saga;

    for (int i = 0; i < s->step_count; i++) {
        s->current_step = i;
        saga_step_t* step = &s->steps[i];

        step->status = SAGA_STEP_PENDING;
        int rc = step->action(step->context, &step->result);

        if (rc == 0) {
            step->status = SAGA_STEP_SUCCEEDED;
        } else {
            step->status = SAGA_STEP_FAILED;

            /* 回滚 */
            for (int j = i - 1; j >= 0; j--) {
                if (s->steps[j].compensate) {
                    s->steps[j].compensate(s->steps[j].context);
                    s->steps[j].status = SAGA_STEP_COMPENSATED;
                }
            }

            if (s->on_failure) {
                s->on_failure(i, step->result);
            }
            return;
        }
    }

    if (s->on_success) {
        s->on_success(s->steps[s->step_count - 1].result);
    }
}

/* 订单Saga示例 */
typedef struct {
    int order_id;
    int inventory_reserved;
    int payment_made;
    int notification_sent;
} order_context_t;

int reserve_inventory(void* ctx, void** result) {
    order_context_t* c = ctx;
    /* 调用库存服务 */
    c->inventory_reserved = 1;
    return 0;
}

int cancel_inventory(void* ctx) {
    order_context_t* c = ctx;
    if (c->inventory_reserved) {
        /* 释放库存 */
    }
    return 0;
}

int process_payment(void* ctx, void** result) {
    order_context_t* c = ctx;
    /* 调用支付服务 */
    c->payment_made = 1;
    return 0;
}

int refund_payment(void* ctx) {
    order_context_t* c = ctx;
    if (c->payment_made) {
        /* 退款 */
    }
    return 0;
}

saga_t* create_order_saga(order_context_t* ctx) {
    saga_t* saga = calloc(1, sizeof(saga_t));
    saga->steps = calloc(3, sizeof(saga_step_t));
    saga->step_count = 3;

    saga->steps[0] = (saga_step_t){
        .name = "reserve_inventory",
        .action = reserve_inventory,
        .compensate = cancel_inventory,
        .context = ctx
    };

    saga->steps[1] = (saga_step_t){
        .name = "process_payment",
        .action = process_payment,
        .compensate = refund_payment,
        .context = ctx
    };

    saga->steps[2] = (saga_step_t){
        .name = "send_notification",
        .action = send_notification,
        .compensate = NULL,  /* 不需要补偿 */
        .context = ctx
    };

    return saga;
}
```

---

## CQRS模式

```c
/* CQRS: 命令查询职责分离 */

/* 命令端 (写模型) */
typedef struct {
    int id;
    char name[64];
    double balance;
    int version;  /* 乐观锁 */
} account_write_model_t;

typedef enum {
    CMD_CREATE_ACCOUNT,
    CMD_DEPOSIT,
    CMD_WITHDRAW,
    CMD_TRANSFER
} command_type_t;

typedef struct {
    command_type_t type;
    int account_id;
    union {
        struct { char name[64]; } create;
        struct { double amount; } deposit;
        struct { double amount; } withdraw;
        struct { int to_id; double amount; } transfer;
    } data;
    int expected_version;  /* 用于乐观锁 */
} command_t;

/* 命令处理器 */
int command_handler(void* store, command_t* cmd) {
    switch (cmd->type) {
        case CMD_DEPOSIT: {
            account_write_model_t* acc = store_get(store, cmd->account_id);
            if (!acc) return -ENOENT;

            /* 版本检查 */
            if (acc->version != cmd->expected_version) {
                return -EAGAIN;  /* 并发冲突 */
            }

            acc->balance += cmd->data.deposit.amount;
            acc->version++;

            /* 发布事件 */
            event_t ev = {
                .type = EVENT_DEPOSITED,
                .account_id = cmd->account_id,
                .amount = cmd->data.deposit.amount,
                .version = acc->version
            };
            event_store_append(store, &ev);
            return 0;
        }
        /* ... */
    }
}

/* 查询端 (读模型) */
typedef struct {
    int id;
    char name[64];
    double balance;
    /* 可能冗余数据以优化查询 */
    double total_deposits;
    double total_withdrawals;
} account_read_model_t;

/* 物化视图 */
typedef struct {
    account_read_model_t* accounts;
    int count;
    mtx_t lock;
} account_query_store_t;

/* 查询 */
account_read_model_t* query_account(account_query_store_t* store, int id) {
    mtx_lock(&store->lock);
    /* 查找并返回 */
    for (int i = 0; i < store->count; i++) {
        if (store->accounts[i].id == id) {
            mtx_unlock(&store->lock);
            return &store->accounts[i];
        }
    }
    mtx_unlock(&store->lock);
    return NULL;
}

/* 查询所有余额大于X的账户 */
account_read_model_t** query_accounts_by_balance(
    account_query_store_t* store,
    double min_balance,
    int* count) {
    /* 已优化的读模型可以直接查询 */
}

/* 事件处理器更新读模型 */
void on_event(event_t* ev) {
    switch (ev->type) {
        case EVENT_DEPOSITED:
            update_read_model_deposit(ev->account_id, ev->amount);
            break;
        case EVENT_WITHDRAWN:
            update_read_model_withdraw(ev->account_id, ev->amount);
            break;
    }
}
```

---

## 参考

- [Reactor Pattern](https://www.dre.vanderbilt.edu/~schmidt/PDF/Reactor.pdf)
- [CQRS by Martin Fowler](https://martinfowler.com/bliki/CQRS.html)
- [Saga Pattern](https://microservices.io/patterns/data/saga.html)


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
