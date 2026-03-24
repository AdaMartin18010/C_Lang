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
﻿# C语言实现 Actor 模型

## 目录

- [C语言实现 Actor 模型](#c语言实现-actor-模型)
  - [目录](#目录)
  - [Actor模型概述](#actor模型概述)
    - [什么是Actor模型](#什么是actor模型)
    - [Actor的特性](#actor的特性)
  - [核心概念](#核心概念)
    - [1. 邮箱 (Mailbox)](#1-邮箱-mailbox)
    - [2. Actor引用](#2-actor引用)
    - [3. 消息定义](#3-消息定义)
  - [基础实现](#基础实现)
    - [Actor系统](#actor系统)
    - [Actor生命周期](#actor生命周期)
  - [消息传递系统](#消息传递系统)
    - [发送消息](#发送消息)
    - [消息处理循环](#消息处理循环)
  - [监督树](#监督树)
    - [监督策略](#监督策略)
    - [监督者Actor](#监督者actor)
  - [调度器实现](#调度器实现)
    - [工作窃取调度器](#工作窃取调度器)
  - [高级特性](#高级特性)
    - [热代码升级](#热代码升级)
    - [分布式Actor](#分布式actor)
  - [完整示例](#完整示例)
    - [计数器Actor](#计数器actor)
    - [请求-响应模式](#请求-响应模式)
  - [总结](#总结)
  - [参考资源](#参考资源)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)

---

## Actor模型概述

### 什么是Actor模型

```
Actor模型是一种并发计算模型，由Carl Hewitt于1973年提出。

核心原则:
1. 一切皆Actor - Actor是并发计算的基本单元
2. 不共享状态 - Actor之间不共享内存
3. 异步消息传递 - Actor通过异步消息通信
4. 故障隔离 - 每个Actor独立失败和重启

对比其他并发模型:
- 线程+锁: 共享内存，容易死锁
- CSP (Go): 同步channel，显式同步
- Actor: 异步消息，解耦发送者和接收者

成功应用:
- Erlang/OTP: 电信系统，99.9999999%可用性
- Akka: JVM平台，分布式系统
- Orleans: .NET，游戏后端
- Pony: 编程语言，能力安全
```

### Actor的特性

```c
/* Actor的基本特性 */

typedef struct actor {
    /* 唯一标识 */
    aid_t id;

    /* 邮箱 (消息队列) */
    mailbox_t* mailbox;

    /* 状态和行为 */
    void* state;
    void (*behaviour)(struct actor*, message_t*);

    /* 监督 */
    struct actor* parent;       /* 监督者 */
    struct actor** children;    /* 被监督者 */
    int child_count;

    /* 生命周期 */
    atomic_int status;          /* running/paused/stopped */
} actor_t;

/* Actor处理消息 */
void actor_receive(actor_t* self, message_t* msg) {
    /* 根据消息类型分发 */
    switch (msg->type) {
        case MSG_USER:
            self->behaviour(self, msg);  /* 用户定义行为 */
            break;
        case MSG_SUPERVISE:
            handle_supervision(self, msg);  /* 监督消息 */
            break;
        case MSG_SYSTEM:
            handle_system(self, msg);  /* 系统消息 */
            break;
    }
}
```

---

## 核心概念

### 1. 邮箱 (Mailbox)

```c
#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>

/* 无锁邮箱实现 */

typedef struct msg_node {
    message_t msg;
    _Atomic(struct msg_node*) next;
} msg_node_t;

typedef struct {
    _Atomic(msg_node_t*) head;  /* 出队端 */
    _Atomic(msg_node_t*) tail;  /* 入队端 */
    atomic_int count;
    atomic_int dropped;         /* 丢弃计数 */
    size_t max_size;            /* 最大容量 */
} mailbox_t;

void mailbox_init(mailbox_t* box, size_t max_size) {
    msg_node_t* dummy = malloc(sizeof(msg_node_t));
    dummy->next = NULL;

    atomic_init(&box->head, dummy);
    atomic_init(&box->tail, dummy);
    atomic_init(&box->count, 0);
    atomic_init(&box->dropped, 0);
    box->max_size = max_size;
}

/* 发送消息 (任意线程) */
bool mailbox_send(mailbox_t* box, message_t* msg) {
    /* 检查容量 */
    int current = atomic_load_explicit(&box->count, memory_order_relaxed);
    if (box->max_size > 0 && current >= (int)box->max_size) {
        atomic_fetch_add_explicit(&box->dropped, 1, memory_order_relaxed);
        return false;  /* 邮箱满 */
    }

    msg_node_t* node = malloc(sizeof(msg_node_t));
    node->msg = *msg;
    atomic_init(&node->next, NULL);

    msg_node_t* tail;
    msg_node_t* next;

    /* Michael-Scott 队列入队 */
    while (1) {
        tail = atomic_load_explicit(&box->tail, memory_order_acquire);
        next = atomic_load_explicit(&tail->next, memory_order_acquire);

        if (tail == atomic_load_explicit(&box->tail, memory_order_relaxed)) {
            if (next == NULL) {
                if (atomic_compare_exchange_weak_explicit(
                    &tail->next, &next, node,
                    memory_order_release, memory_order_relaxed)) {
                    break;
                }
            } else {
                atomic_compare_exchange_weak_explicit(
                    &box->tail, &tail, next,
                    memory_order_release, memory_order_relaxed);
            }
        }
    }

    atomic_compare_exchange_weak_explicit(
        &box->tail, &tail, node,
        memory_order_release, memory_order_relaxed);

    atomic_fetch_add_explicit(&box->count, 1, memory_order_release);
    return true;
}

/* 接收消息 (仅Actor线程) */
bool mailbox_recv(mailbox_t* box, message_t* msg, int timeout_ms) {
    msg_node_t* head;
    msg_node_t* next;

    /* 非阻塞尝试 */
    head = atomic_load_explicit(&box->head, memory_order_acquire);
    next = atomic_load_explicit(&head->next, memory_order_acquire);

    if (next == NULL) {
        return false;  /* 空 */
    }

    *msg = next->msg;
    atomic_compare_exchange_strong_explicit(
        &box->head, &head, next,
        memory_order_release, memory_order_relaxed);

    free(head);  /* 旧dummy节点 */
    atomic_fetch_sub_explicit(&box->count, 1, memory_order_release);
    return true;
}
```

### 2. Actor引用

```c
/* Actor引用 - 可本地可远程 */

typedef uint64_t aid_t;  /* Actor ID */

#define AID_LOCAL_BITS  48
#define AID_NODE_MASK   0xFFFF000000000000ULL
#define AID_LOCAL_MASK  0x0000FFFFFFFFFFFFULL

typedef struct actor_ref {
    aid_t id;

    /* 如果是本地Actor，直接指针 */
    actor_t* local;

    /* 如果是远程Actor，网络信息 */
    struct {
        uint16_t node_id;
        char address[64];
        int port;
    } remote;
} actor_ref_t;

/* 创建本地Actor引用 */
actor_ref_t local_ref(actor_t* actor) {
    return (actor_ref_t){
        .id = actor->id,
        .local = actor,
        .remote = {0}
    };
}

/* 发送消息到引用 */
void actor_ref_send(actor_ref_t* ref, message_t* msg) {
    if (ref->local != NULL) {
        /* 本地发送 */
        mailbox_send(ref->local->mailbox, msg);
    } else {
        /* 远程发送 - 序列化后网络发送 */
        remote_send(ref->id, ref->remote.node_id, msg);
    }
}
```

### 3. 消息定义

```c
/* 消息类型 */
typedef enum {
    MSG_USER = 0,           /* 用户消息 */
    MSG_SPAWN,              /* 创建子Actor */
    MSG_KILL,               /* 终止Actor */
    MSG_LINK,               /* 建立监督关系 */
    MSG_UNLINK,             /* 解除监督 */
    MSG_EXIT,               /* Actor退出通知 */
    MSG_SUPERVISE,          /* 监督指令 */
} msg_type_t;

/* 消息结构 */
typedef struct message {
    msg_type_t type;
    aid_t sender;           /* 发送者 */
    aid_t target;           /* 目标 */

    union {
        /* 用户数据 */
        struct {
            int user_type;
            void* data;
            size_t size;
        } user;

        /* 监督数据 */
        struct {
            aid_t child;
            int reason;     /* 退出原因 */
        } exit;

        /* 创建Actor */
        struct {
            void* (*behaviour)(void*);  /* 行为函数 */
            void* arg;
            actor_ref_t* reply_to;
        } spawn;
    } payload;
} message_t;

/* 创建消息 */
message_t make_message(msg_type_t type, aid_t sender, aid_t target) {
    return (message_t){
        .type = type,
        .sender = sender,
        .target = target,
        .payload = {0}
    };
}
```

---

## 基础实现

### Actor系统

```c
#include <threads.h>
#include <string.h>

#define MAX_ACTORS 65536
#define MAX_WORKERS 16

/* 全局Actor表 */
typedef struct {
    mtx_t lock;
    actor_t* actors[MAX_ACTORS];
    atomic_aid_t next_id;
    atomic_int count;
} actor_system_t;

static actor_system_t g_system;

/* 初始化Actor系统 */
bool actor_system_init(void) {
    memset(&g_system, 0, sizeof(g_system));
    mtx_init(&g_system.lock, mtx_plain);
    atomic_init(&g_system.next_id, 1);
    atomic_init(&g_system.count, 0);
    return true;
}

/* 注册Actor */
bool actor_register(actor_t* actor) {
    mtx_lock(&g_system.lock);

    if (g_system.actors[actor->id % MAX_ACTORS] != NULL) {
        mtx_unlock(&g_system.lock);
        return false;  /* ID冲突 */
    }

    g_system.actors[actor->id % MAX_ACTORS] = actor;
    atomic_fetch_add(&g_system.count, 1);

    mtx_unlock(&g_system.lock);
    return true;
}

/* 查找Actor */
actor_t* actor_find(aid_t id) {
    mtx_lock(&g_system.lock);
    actor_t* actor = g_system.actors[id % MAX_ACTORS];
    if (actor && actor->id == id) {
        mtx_unlock(&g_system.lock);
        return actor;
    }
    mtx_unlock(&g_system.lock);
    return NULL;
}

/* 注销Actor */
void actor_unregister(aid_t id) {
    mtx_lock(&g_system.lock);
    g_system.actors[id % MAX_ACTORS] = NULL;
    atomic_fetch_sub(&g_system.count, 1);
    mtx_unlock(&g_system.lock);
}
```

### Actor生命周期

```c
typedef enum {
    ACTOR_INIT,
    ACTOR_RUNNING,
    ACTOR_PAUSED,
    ACTOR_STOPPING,
    ACTOR_DEAD
} actor_status_t;

/* 创建Actor */
actor_t* actor_spawn(void (*behaviour)(actor_t*, message_t*),
                     void* initial_state,
                     actor_t* parent) {
    actor_t* actor = calloc(1, sizeof(actor_t));

    actor->id = atomic_fetch_add(&g_system.next_id, 1);
    actor->state = initial_state;
    actor->behaviour = behaviour;
    actor->parent = parent;
    actor->children = NULL;
    actor->child_count = 0;
    atomic_init(&actor->status, ACTOR_INIT);

    actor->mailbox = malloc(sizeof(mailbox_t));
    mailbox_init(actor->mailbox, 10000);  /* 默认10k消息上限 */

    /* 添加到父Actor的监督列表 */
    if (parent != NULL) {
        mtx_lock(&parent->lock);
        parent->children = realloc(parent->children,
                                   sizeof(actor_t*) * (parent->child_count + 1));
        parent->children[parent->child_count++] = actor;
        mtx_unlock(&parent->lock);
    }

    actor_register(actor);
    atomic_store(&actor->status, ACTOR_RUNNING);

    return actor;
}

/* 终止Actor */
void actor_kill(actor_t* actor, int reason) {
    actor_status_t old = ACTOR_RUNNING;
    if (!atomic_compare_exchange_strong(&actor->status, &old, ACTOR_STOPPING)) {
        return;  /* 已经在停止 */
    }

    /* 通知父Actor */
    if (actor->parent != NULL) {
        message_t msg = make_message(MSG_EXIT, actor->id, actor->parent->id);
        msg.payload.exit.child = actor->id;
        msg.payload.exit.reason = reason;
        mailbox_send(actor->parent->mailbox, &msg);
    }

    /* 终止所有子Actor */
    mtx_lock(&actor->lock);
    for (int i = 0; i < actor->child_count; i++) {
        actor_kill(actor->children[i], REASON_PARENT_EXIT);
    }
    mtx_unlock(&actor->lock);

    atomic_store(&actor->status, ACTOR_DEAD);
    actor_unregister(actor->id);
}

/* 链接Actor (监督关系) */
void actor_link(actor_t* a, actor_t* b) {
    /* 互相监控 */
    message_t msg = make_message(MSG_LINK, a->id, b->id);
    mailbox_send(b->mailbox, &msg);

    msg.target = a->id;
    msg.sender = b->id;
    mailbox_send(a->mailbox, &msg);
}
```

---

## 消息传递系统

### 发送消息

```c
/* 发送消息到Actor */
bool send(aid_t target_id, message_t* msg) {
    actor_t* target = actor_find(target_id);
    if (target == NULL) {
        return false;  /* Actor不存在 */
    }

    msg->target = target_id;
    return mailbox_send(target->mailbox, msg);
}

/* 带超时的发送 */
bool send_timeout(aid_t target_id, message_t* msg, int timeout_ms) {
    /* 如果邮箱满，等待 */
    actor_t* target = actor_find(target_id);
    if (target == NULL) return false;

    int waited = 0;
    while (!mailbox_send(target->mailbox, msg)) {
        if (waited >= timeout_ms) return false;
        thrd_sleep(&(struct timespec){0, 10000000}, NULL);  /* 10ms */
        waited += 10;
    }
    return true;
}

/* 发送并等待回复 */
message_t* send_receive(aid_t target_id, message_t* msg, int timeout_ms) {
    /* 创建临时邮箱 */
    mailbox_t reply_box;
    mailbox_init(&reply_box, 1);

    /* 在消息中包含回复地址 */
    msg->reply_to = &reply_box;

    if (!send(target_id, msg)) {
        return NULL;
    }

    /* 等待回复 */
    message_t* reply = malloc(sizeof(message_t));
    if (!mailbox_recv(&reply_box, reply, timeout_ms)) {
        free(reply);
        return NULL;
    }

    return reply;
}
```

### 消息处理循环

```c
/* Actor的主循环 */
void actor_loop(actor_t* actor) {
    message_t msg;

    while (atomic_load(&actor->status) == ACTOR_RUNNING) {
        /* 接收消息 */
        if (mailbox_recv(actor->mailbox, &msg, 100)) {
            /* 处理消息 */
            switch (msg.type) {
                case MSG_USER:
                    actor->behaviour(actor, &msg);
                    break;

                case MSG_SPAWN:
                    /* 创建子Actor */
                    actor_spawn(msg.payload.spawn.behaviour,
                               msg.payload.spawn.arg,
                               actor);
                    break;

                case MSG_KILL:
                    actor_kill(actor, REASON_NORMAL);
                    return;

                case MSG_EXIT:
                    /* 子Actor退出 */
                    handle_child_exit(actor, &msg);
                    break;

                case MSG_LINK:
                    /* 建立链接 */
                    break;

                default:
                    break;
            }

            /* 清理消息资源 */
            if (msg.user.data != NULL && msg.user.free_data) {
                free(msg.user.data);
            }
        }
    }
}
```

---

## 监督树

### 监督策略

```c
typedef enum {
    STRATEGY_ONE_FOR_ONE,    /* 一个失败只重启它 */
    STRATEGY_ONE_FOR_ALL,    /* 一个失败重启所有 */
    STRATEGY_REST_FOR_ONE    /* 一个失败重启后面的 */
} supervision_strategy_t;

typedef struct {
    supervision_strategy_t strategy;
    int max_restarts;        /* 最大重启次数 */
    int restart_window;      /* 时间窗口(秒) */
    int backoff_base;        /* 退避基数(ms) */
} supervisor_spec_t;

typedef struct {
    aid_t child_id;
    int restart_count;
    time_t last_restart;
    supervisor_spec_t spec;
} child_spec_t;

/* 处理子Actor退出 */
void handle_child_exit(actor_t* supervisor, message_t* msg) {
    aid_t child_id = msg->payload.exit.child;
    int reason = msg->payload.exit.reason;

    /* 根据退出原因决定 */
    if (reason == REASON_NORMAL) {
        /* 正常退出，不需要重启 */
        remove_child(supervisor, child_id);
        return;
    }

    /* 查找子Actor配置 */
    child_spec_t* spec = find_child_spec(supervisor, child_id);
    if (spec == NULL) return;

    /* 检查重启频率 */
    time_t now = time(NULL);
    if (now - spec->last_restart < spec->spec.restart_window) {
        spec->restart_count++;
        if (spec->restart_count > spec->spec.max_restarts) {
            /* 重启过于频繁，停止监督 */
            actor_kill(supervisor, REASON_TOO_MANY_RESTARTS);
            return;
        }
    } else {
        spec->restart_count = 1;
    }

    spec->last_restart = now;

    /* 根据策略处理 */
    switch (spec->spec.strategy) {
        case STRATEGY_ONE_FOR_ONE:
            /* 只重启失败的子Actor */
            restart_child(supervisor, child_id);
            break;

        case STRATEGY_ONE_FOR_ALL:
            /* 停止所有子Actor，然后全部重启 */
            for (int i = 0; i < supervisor->child_count; i++) {
                actor_kill(supervisor->children[i], REASON_SUPERVISOR_RESTART);
            }
            restart_all_children(supervisor);
            break;

        case STRATEGY_REST_FOR_ONE:
            /* 停止并重启失败Actor之后的所有Actor */
            restart_child_and_later(supervisor, child_id);
            break;
    }
}

/* 带退避的重启 */
void restart_child(actor_t* supervisor, aid_t child_id) {
    child_spec_t* spec = find_child_spec(supervisor, child_id);

    /* 计算退避时间 */
    int backoff = spec->spec.backoff_base * (1 << spec->restart_count);
    if (backoff > 60000) backoff = 60000;  /* 最大60秒 */

    thrd_sleep(&(struct timespec){backoff / 1000,
                                   (backoff % 1000) * 1000000}, NULL);

    /* 重新创建Actor */
    actor_t* new_child = actor_spawn(spec->behaviour, spec->initial_state,
                                     supervisor);
    spec->child_id = new_child->id;
}
```

### 监督者Actor

```c
/* 专门的监督者行为 */
void supervisor_behaviour(actor_t* self, message_t* msg) {
    switch (msg->type) {
        case MSG_EXIT:
            handle_child_exit(self, msg);
            break;

        case MSG_SPAWN_MANAGED: {
            /* 创建受监督的Actor */
            actor_t* child = actor_spawn(
                msg->payload.spawn.behaviour,
                msg->payload.spawn.arg,
                self  /* 监督者是当前Actor */
            );

            /* 记录配置 */
            add_child_spec(self, child->id, &msg->payload.spawn.spec);

            /* 回复创建的Actor ID */
            if (msg->reply_to) {
                message_t reply = {0};
                reply.type = MSG_SPAWN_RESULT;
                reply.payload.spawn_result.id = child->id;
                mailbox_send(msg->reply_to, &reply);
            }
            break;
        }
    }
}

/* 创建根监督者 */
actor_t* create_root_supervisor(supervisor_spec_t* spec) {
    return actor_spawn(supervisor_behaviour, spec, NULL);
}
```

---

## 调度器实现

### 工作窃取调度器

```c
#include <threads.h>

#define NUM_WORKERS 4
#define LOCAL_QUEUE_SIZE 256

typedef struct worker {
    thrd_t thread;
    int id;

    /* 本地队列 */
    actor_t* local_queue[LOCAL_QUEUE_SIZE];
    _Atomic(int) head;
    _Atomic(int) tail;

    /* 随机数种子 */
    unsigned int rand_seed;

    struct worker* next;
} worker_t;

static worker_t* workers[NUM_WORKERS];
static _Atomic(int) next_worker_id = 0;

/* 提交Actor到调度器 */
void schedule_actor(actor_t* actor) {
    int worker_id = atomic_fetch_add(&next_worker_id, 1) % NUM_WORKERS;
    worker_t* w = workers[worker_id];

    /* 添加到工作者队列 */
    int tail = atomic_load_explicit(&w->tail, memory_order_relaxed);
    w->local_queue[tail % LOCAL_QUEUE_SIZE] = actor;
    atomic_store_explicit(&w->tail, tail + 1, memory_order_release);
}

/* 工作者线程 */
static int worker_thread(void* arg) {
    worker_t* self = arg;

    while (1) {
        actor_t* actor = NULL;

        /* 尝试从本地队列获取 */
        int head = atomic_load_explicit(&self->head, memory_order_relaxed);
        int tail = atomic_load_explicit(&self->tail, memory_order_acquire);

        if (head < tail) {
            /* 有本地任务 */
            actor = self->local_queue[head % LOCAL_QUEUE_SIZE];
            atomic_store_explicit(&self->head, head + 1, memory_order_release);
        } else {
            /* 尝试窃取 */
            actor = steal_actor(self);
        }

        if (actor != NULL) {
            /* 执行Actor */
            process_actor(actor);
        } else {
            /* 没有任务，短暂等待 */
            thrd_yield();
        }
    }

    return 0;
}

/* 工作窃取 */
actor_t* steal_actor(worker_t* thief) {
    /* 随机选择一个受害者 */
    int victim_id = rand_r(&thief->rand_seed) % NUM_WORKERS;
    worker_t* victim = workers[victim_id];

    if (victim == thief) return NULL;

    /* 从受害者尾部窃取 */
    int tail = atomic_load_explicit(&victim->tail, memory_order_relaxed) - 1;
    atomic_store_explicit(&victim->tail, tail, memory_order_relaxed);
    atomic_thread_fence(memory_order_seq_cst);
    int head = atomic_load_explicit(&victim->head, memory_order_relaxed);

    if (head <= tail) {
        actor_t* actor = victim->local_queue[tail % LOCAL_QUEUE_SIZE];

        if (head == tail) {
            /* 最后一个，竞争 */
            if (!atomic_compare_exchange_strong_explicit(
                &victim->head, &head, head + 1,
                memory_order_seq_cst, memory_order_relaxed)) {
                /* 竞争失败 */
                atomic_store_explicit(&victim->tail, tail + 1,
                                       memory_order_relaxed);
                return NULL;
            }
            atomic_store_explicit(&victim->tail, tail + 1,
                                   memory_order_relaxed);
        }
        return actor;
    } else {
        /* 空队列 */
        atomic_store_explicit(&victim->tail, tail + 1, memory_order_relaxed);
        return NULL;
    }
}

/* 处理单个Actor */
void process_actor(actor_t* actor) {
    message_t msg;
    int processed = 0;

    /* 处理一批消息 */
    while (processed < 100 &&
           mailbox_recv(actor->mailbox, &msg, 0)) {
        actor->behaviour(actor, &msg);
        processed++;
    }

    /* 重新调度Actor */
    if (atomic_load(&actor->status) == ACTOR_RUNNING &&
        atomic_load(&actor->mailbox->count) > 0) {
        schedule_actor(actor);
    }
}
```

---

## 高级特性

### 热代码升级

```c
/* Actor行为版本控制 */
typedef struct behaviour_version {
    int version;
    void (*handler)(actor_t*, message_t*);
    struct behaviour_version* next;
} behaviour_version_t;

typedef struct {
    atomic_int current_version;
    behaviour_version_t* versions;
} hot_upgrade_t;

/* 升级Actor行为 */
void upgrade_actor_behaviour(aid_t actor_id,
                              void (*new_behaviour)(actor_t*, message_t*),
                              int new_version) {
    actor_t* actor = actor_find(actor_id);
    if (actor == NULL) return;

    /* 发送升级消息 */
    message_t msg = {
        .type = MSG_UPGRADE,
        .payload.upgrade.version = new_version,
        .payload.upgrade.handler = new_behaviour
    };

    mailbox_send(actor->mailbox, &msg);
}

/* Actor处理升级 */
void handle_upgrade(actor_t* actor, message_t* msg) {
    /* 保存状态 */
    void* state = actor->state;

    /* 切换行为 */
    actor->behaviour = msg->payload.upgrade.handler;

    /* 状态迁移 */
    actor->state = migrate_state(state, msg->payload.upgrade.version);
}
```

### 分布式Actor

```c
/* 节点发现 */
typedef struct node {
    uint16_t id;
    char address[64];
    int port;
    _Atomic(bool) connected;
} node_t;

/* 远程消息序列化 */
void remote_send(aid_t target, uint16_t node_id, message_t* msg) {
    node_t* node = find_node(node_id);
    if (node == NULL || !atomic_load(&node->connected)) {
        return;
    }

    /* 序列化消息 */
    uint8_t buffer[4096];
    size_t len = serialize_message(msg, buffer, sizeof(buffer));

    /* 发送 */
    send_to_node(node, buffer, len);
}

/* 接收远程消息 */
void handle_remote_message(node_t* from, uint8_t* data, size_t len) {
    message_t msg;
    deserialize_message(&msg, data, len);

    /* 路由到本地Actor */
    actor_t* target = actor_find(msg.target);
    if (target != NULL) {
        mailbox_send(target->mailbox, &msg);
    }
}

/* 跨节点Actor发现 */
actor_ref_t lookup_remote_actor(const char* name, uint16_t node_id) {
    /* 发送查询 */
    message_t query = {
        .type = MSG_LOOKUP,
        .payload.lookup.name = name
    };

    /* 等待响应 */
    message_t* response = send_receive(name_server_id, &query, 5000);

    if (response && response->type == MSG_LOOKUP_RESULT) {
        return response->payload.lookup_result.ref;
    }

    return (actor_ref_t){0};
}
```

---

## 完整示例

### 计数器Actor

```c
/* 简单的计数器Actor */

typedef struct {
    int count;
} counter_state_t;

void counter_behaviour(actor_t* self, message_t* msg) {
    counter_state_t* state = self->state;

    switch (msg->payload.user.user_type) {
        case CMD_INCREMENT:
            state->count++;
            break;

        case CMD_GET: {
            /* 发送当前值给请求者 */
            message_t reply = {
                .type = MSG_USER,
                .sender = self->id,
                .payload.user.user_type = RESP_COUNT,
                .payload.user.data = &state->count,
                .payload.user.size = sizeof(int)
            };
            send(msg->sender, &reply);
            break;
        }
    }
}

/* 使用 */
void example_counter(void) {
    /* 创建监督者 */
    supervisor_spec_t spec = {
        .strategy = STRATEGY_ONE_FOR_ONE,
        .max_restarts = 5,
        .restart_window = 60
    };
    actor_t* supervisor = create_root_supervisor(&spec);

    /* 创建计数器 */
    counter_state_t initial = {0};
    actor_t* counter = actor_spawn(counter_behaviour, &initial, supervisor);

    /* 发送增加消息 */
    message_t inc = {
        .type = MSG_USER,
        .payload.user.user_type = CMD_INCREMENT
    };
    send(counter->id, &inc);
    send(counter->id, &inc);
    send(counter->id, &inc);

    /* 获取值 */
    message_t get = {
        .type = MSG_USER,
        .sender = self_id,  /* 自己的Actor ID */
        .payload.user.user_type = CMD_GET
    };
    send(counter->id, &get);

    /* 等待回复 */
    message_t reply;
    if (receive(&reply, 1000)) {
        int* count = reply.payload.user.data;
        printf("Count: %d\n", *count);  /* 输出 3 */
    }
}
```

### 请求-响应模式

```c
/* GenServer风格的请求响应 */

typedef struct {
    aid_t caller;
    int request_id;
    void* data;
} request_ctx_t;

void server_behaviour(actor_t* self, message_t* msg) {
    switch (msg->type) {
        case MSG_CALL: {
            /* 同步调用 */
            request_ctx_t ctx = {
                .caller = msg->sender,
                .request_id = msg->payload.call.request_id,
                .data = msg->payload.call.data
            };

            /* 处理请求 */
            void* result = handle_call(self, &ctx);

            /* 发送回复 */
            message_t reply = {
                .type = MSG_REPLY,
                .target = ctx.caller,
                .payload.reply.request_id = ctx.request_id,
                .payload.reply.result = result
            };
            send(ctx.caller, &reply);
            break;
        }

        case MSG_CAST: {
            /* 异步调用，无回复 */
            handle_cast(self, msg->payload.cast.data);
            break;
        }
    }
}

/* 客户端调用 */
void* call(aid_t server, void* request, int timeout_ms) {
    static atomic_int request_id = 0;

    message_t msg = {
        .type = MSG_CALL,
        .sender = self_id,
        .payload.call.request_id = atomic_fetch_add(&request_id, 1),
        .payload.call.data = request
    };

    /* 发送并等待 */
    send(server, &msg);

    message_t reply;
    while (receive(&reply, timeout_ms)) {
        if (reply.type == MSG_REPLY &&
            reply.payload.reply.request_id == msg.payload.call.request_id) {
            return reply.payload.reply.result;
        }
        /* 其他消息入队列稍后处理 */
        queue_message(&reply);
    }

    return NULL;  /* 超时 */
}

void cast(aid_t server, void* request) {
    message_t msg = {
        .type = MSG_CAST,
        .sender = self_id,
        .payload.cast.data = request
    };
    send(server, &msg);
}
```

---

## 总结

C语言实现Actor模型的关键要点：

1. **消息队列**: 使用无锁Michael-Scott队列实现高性能邮箱
2. **内存管理**: Hazard Pointers处理延迟释放
3. **监督树**: 实现容错和自动恢复
4. **调度**: 工作窃取实现负载均衡
5. **设计**: 避免共享状态，所有通信通过消息

这种实现可以达到接近Erlang的并发模型，同时保持C语言的高性能。

---

## 参考资源

- [A Universal Modular ACTOR Formalism](https://dl.acm.org/doi/10.1145/1625143.1624804)
- [Erlang OTP Design Principles](https://www.erlang.org/doc/design_principles/sup_princ.html)
- [Akka Actor Model](https://doc.akka.io/docs/akka/current/typed/actors.html)
- [Orleans: Distributed Virtual Actors](https://dotnet.github.io/orleans/)


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
