# 工作流与状态机

## 概述

工作流与状态机是描述和控制复杂系统行为的核心抽象。状态机通过定义有限状态和转移规则来管理对象生命周期，而工作流引擎则协调多步骤业务流程的执行。在C语言系统编程中，这两种模式广泛应用于协议实现、业务逻辑编排和设备控制。

---

## 1. 状态机模式 (State Machine Pattern)

### 1.1 状态机分类

```
状态机类型:

┌─────────────────────────────────────────────────────────────┐
│                                                             │
│   有限状态机 (FSM)                                           │
│        │                                                    │
│   ┌────┴────┬────────────────┐                             │
│   │         │                │                             │
│   ▼         ▼                ▼                             │
│  Mealy    Moore           层次状态机                        │
│  米利型   摩尔型          (Hierarchical)                    │
│  输出依赖  输出仅依赖      支持子状态机                      │
│  输入和状态 当前状态                                      │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 经典状态机实现

```c
/* state_machine.h - 通用状态机框架 */
#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <stdint.h>
#include <stdbool.h>

/* 状态句柄前向声明 */
typedef struct state_machine state_machine_t;
typedef struct state state_t;

/* 事件类型 */
typedef uint32_t event_t;

/* 动作函数类型 */
typedef void (*action_t)(state_machine_t *sm, void *ctx);
typedef bool (*guard_t)(state_machine_t *sm, void *ctx);

/* 状态结构 */
struct state {
    const char *name;
    uint32_t id;

    /* 进入/退出动作 */
    action_t on_entry;
    action_t on_exit;

    /* 状态活动 (持续执行) */
    action_t on_run;

    /* 父状态 (层次状态机) */
    state_t *parent;

    /* 子状态机入口 */
    state_t *initial_substate;
};

/* 状态转移 */
typedef struct {
    state_t *source;
    state_t *target;
    event_t event;
    guard_t guard;          /* 守卫条件 */
    action_t action;        /* 转移动作 */
} transition_t;

/* 状态机 */
struct state_machine {
    const char *name;
    state_t *current_state;
    state_t *previous_state;

    /* 状态表 */
    state_t **states;
    uint32_t state_count;
    uint32_t state_capacity;

    /* 转移表 */
    transition_t **transitions;
    uint32_t trans_count;
    uint32_t trans_capacity;

    /* 历史记录 (用于恢复) */
    state_t *history[16];
    uint32_t history_index;

    /* 用户上下文 */
    void *user_ctx;

    /* 状态机数据 */
    uint32_t event_queue[32];
    uint32_t queue_head;
    uint32_t queue_tail;
};

/* API */
state_machine_t *sm_create(const char *name, void *user_ctx);
void sm_destroy(state_machine_t *sm);

int sm_add_state(state_machine_t *sm, state_t *state);
int sm_add_transition(state_machine_t *sm, transition_t *trans);
int sm_set_initial_state(state_machine_t *sm, state_t *state);

int sm_start(state_machine_t *sm);
int sm_dispatch(state_machine_t *sm, event_t event, void *ctx);
int sm_run(state_machine_t *sm);

state_t *sm_get_current_state(state_machine_t *sm);
const char *sm_get_current_state_name(state_machine_t *sm);

#endif /* STATE_MACHINE_H */
```

```c
/* state_machine.c - 状态机核心实现 */
#include "state_machine.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

state_machine_t *sm_create(const char *name, void *user_ctx) {
    state_machine_t *sm = calloc(1, sizeof(state_machine_t));
    if (!sm) return NULL;

    sm->name = name;
    sm->user_ctx = user_ctx;

    /* 初始化状态表 */
    sm->state_capacity = 16;
    sm->states = calloc(sm->state_capacity, sizeof(state_t *));

    /* 初始化转移表 */
    sm->trans_capacity = 32;
    sm->transitions = calloc(sm->trans_capacity, sizeof(transition_t *));

    return sm;
}

int sm_add_state(state_machine_t *sm, state_t *state) {
    if (!sm || !state) return -1;

    if (sm->state_count >= sm->state_capacity) {
        /* 扩容 */
        size_t new_cap = sm->state_capacity * 2;
        state_t **new_states = realloc(sm->states, new_cap * sizeof(state_t *));
        if (!new_states) return -1;
        sm->states = new_states;
        sm->state_capacity = new_cap;
    }

    sm->states[sm->state_count++] = state;
    return 0;
}

int sm_add_transition(state_machine_t *sm, transition_t *trans) {
    if (!sm || !trans) return -1;

    if (sm->trans_count >= sm->trans_capacity) {
        size_t new_cap = sm->trans_capacity * 2;
        transition_t **new_trans = realloc(sm->transitions, new_cap * sizeof(transition_t *));
        if (!new_trans) return -1;
        sm->transitions = new_trans;
        sm->trans_capacity = new_cap;
    }

    sm->transitions[sm->trans_count++] = trans;
    return 0;
}

/* 查找匹配的转移 */
static transition_t *find_transition(state_machine_t *sm, event_t event) {
    for (uint32_t i = 0; i < sm->trans_count; i++) {
        transition_t *trans = sm->transitions[i];
        if (trans->source == sm->current_state && trans->event == event) {
            /* 检查守卫条件 */
            if (!trans->guard || trans->guard(sm, sm->user_ctx)) {
                return trans;
            }
        }
    }
    return NULL;
}

/* 执行状态转移 */
static int execute_transition(state_machine_t *sm, transition_t *trans, void *ctx) {
    printf("[%s] %s --[%u]--> %s\n",
           sm->name,
           trans->source->name,
           trans->event,
           trans->target->name);

    /* 执行源状态退出动作 */
    if (trans->source->on_exit) {
        trans->source->on_exit(sm, ctx);
    }

    /* 执行转移动作 */
    if (trans->action) {
        trans->action(sm, ctx);
    }

    /* 记录历史 */
    sm->previous_state = sm->current_state;
    sm->history[sm->history_index++ % 16] = sm->current_state;

    /* 切换状态 */
    sm->current_state = trans->target;

    /* 执行目标状态进入动作 */
    if (trans->target->on_entry) {
        trans->target->on_entry(sm, ctx);
    }

    return 0;
}

int sm_dispatch(state_machine_t *sm, event_t event, void *ctx) {
    if (!sm || !sm->current_state) return -1;

    transition_t *trans = find_transition(sm, event);
    if (trans) {
        return execute_transition(sm, trans, ctx);
    }

    /* 未找到匹配转移，可能在父状态中处理 */
    printf("[%s] Event %u not handled in state %s\n",
           sm->name, event, sm->current_state->name);
    return -1;
}

int sm_run(state_machine_t *sm) {
    if (!sm || !sm->current_state) return -1;

    /* 执行当前状态的活动 */
    if (sm->current_state->on_run) {
        sm->current_state->on_run(sm, sm->user_ctx);
    }

    /* 处理事件队列 */
    while (sm->queue_head != sm->queue_tail) {
        event_t event = sm->event_queue[sm->queue_head++ % 32];
        sm_dispatch(sm, event, sm->user_ctx);
    }

    return 0;
}
```

### 1.3 TCP连接状态机实例

```c
/* tcp_state_machine.h */
#ifndef TCP_STATE_MACHINE_H
#define TCP_STATE_MACHINE_H

#include "state_machine.h"

/* TCP状态 */
typedef enum {
    TCP_CLOSED = 0,
    TCP_LISTEN,
    TCP_SYN_SENT,
    TCP_SYN_RECEIVED,
    TCP_ESTABLISHED,
    TCP_FIN_WAIT_1,
    TCP_FIN_WAIT_2,
    TCP_CLOSE_WAIT,
    TCP_CLOSING,
    TCP_LAST_ACK,
    TCP_TIME_WAIT
} tcp_state_id_t;

/* TCP事件 */
typedef enum {
    TCP_EVT_OPEN = 1,
    TCP_EVT_SEND,
    TCP_EVT_RECEIVE,
    TCP_EVT_CLOSE,
    TCP_EVT_TIMEOUT,
    TCP_EVT_SYN,
    TCP_EVT_SYN_ACK,
    TCP_EVT_ACK,
    TCP_EVT_FIN,
    TCP_EVT_RST
} tcp_event_t;

/* TCP上下文 */
typedef struct {
    uint32_t local_seq;
    uint32_t remote_seq;
    uint32_t local_port;
    uint32_t remote_port;
    uint32_t retransmit_count;
    void *socket;
} tcp_context_t;

/* 创建TCP状态机 */
state_machine_t *tcp_state_machine_create(tcp_context_t *ctx);

#endif /* TCP_STATE_MACHINE_H */
```

```c
/* tcp_state_machine.c */
#include "tcp_state_machine.h"
#include <stdio.h>

/* 状态定义 */
static state_t state_closed = {
    .name = "CLOSED",
    .id = TCP_CLOSED,
    .on_entry = NULL,
    .on_exit = NULL,
    .on_run = NULL
};

static state_t state_listen = {
    .name = "LISTEN",
    .id = TCP_LISTEN,
    .on_entry = lambda(void, (state_machine_t *sm, void *ctx), {
        printf("TCP: Entering LISTEN state\n");
    }),
    .on_exit = NULL
};

static state_t state_syn_sent = {
    .name = "SYN_SENT",
    .id = TCP_SYN_SENT
};

static state_t state_syn_received = {
    .name = "SYN_RECEIVED",
    .id = TCP_SYN_RECEIVED
};

static state_t state_established = {
    .name = "ESTABLISHED",
    .id = TCP_ESTABLISHED,
    .on_entry = lambda(void, (state_machine_t *sm, void *ctx), {
        printf("TCP: Connection established!\n");
        tcp_context_t *tcp = (tcp_context_t *)ctx;
        tcp->retransmit_count = 0;
    })
};

static state_t state_fin_wait_1 = {
    .name = "FIN_WAIT_1",
    .id = TCP_FIN_WAIT_1
};

static state_t state_fin_wait_2 = {
    .name = "FIN_WAIT_2",
    .id = TCP_FIN_WAIT_2
};

static state_t state_close_wait = {
    .name = "CLOSE_WAIT",
    .id = TCP_CLOSE_WAIT
};

static state_t state_closing = {
    .name = "CLOSING",
    .id = TCP_CLOSING
};

static state_t state_last_ack = {
    .name = "LAST_ACK",
    .id = TCP_LAST_ACK
};

static state_t state_time_wait = {
    .name = "TIME_WAIT",
    .id = TCP_TIME_WAIT,
    .on_entry = lambda(void, (state_machine_t *sm, void *ctx), {
        printf("TCP: Entering TIME_WAIT (2MSL timeout)\n");
    })
};

/* 守卫函数 */
static bool guard_seq_valid(state_machine_t *sm, void *ctx) {
    /* 检查序列号是否有效 */
    tcp_context_t *tcp = (tcp_context_t *)ctx;
    return tcp->retransmit_count < 5;
}

/* 转移定义 */
static transition_t trans_closed_to_syn_sent = {
    .source = &state_closed,
    .target = &state_syn_sent,
    .event = TCP_EVT_OPEN,
    .guard = NULL,
    .action = lambda(void, (state_machine_t *sm, void *ctx), {
        printf("TCP: Sending SYN\n");
    })
};

static transition_t trans_closed_to_listen = {
    .source = &state_closed,
    .target = &state_listen,
    .event = TCP_EVT_OPEN,
    .guard = NULL,
    .action = lambda(void, (state_machine_t *sm, void *ctx), {
        printf("TCP: Starting to listen\n");
    })
};

static transition_t trans_listen_to_syn_received = {
    .source = &state_listen,
    .target = &state_syn_received,
    .event = TCP_EVT_SYN,
    .action = lambda(void, (state_machine_t *sm, void *ctx), {
        printf("TCP: Received SYN, sending SYN-ACK\n");
    })
};

static transition_t trans_syn_sent_to_established = {
    .source = &state_syn_sent,
    .target = &state_established,
    .event = TCP_EVT_SYN_ACK,
    .action = lambda(void, (state_machine_t *sm, void *ctx), {
        printf("TCP: Received SYN-ACK, sending ACK\n");
    })
};

static transition_t trans_syn_received_to_established = {
    .source = &state_syn_received,
    .target = &state_established,
    .event = TCP_EVT_ACK,
    .action = NULL
};

static transition_t trans_established_to_fin_wait_1 = {
    .source = &state_established,
    .target = &state_fin_wait_1,
    .event = TCP_EVT_CLOSE,
    .action = lambda(void, (state_machine_t *sm, void *ctx), {
        printf("TCP: Active close - sending FIN\n");
    })
};

static transition_t trans_established_to_close_wait = {
    .source = &state_established,
    .target = &state_close_wait,
    .event = TCP_EVT_FIN,
    .action = lambda(void, (state_machine_t *sm, void *ctx), {
        printf("TCP: Passive close - received FIN\n");
    })
};

static transition_t trans_fin_wait_1_to_fin_wait_2 = {
    .source = &state_fin_wait_1,
    .target = &state_fin_wait_2,
    .event = TCP_EVT_ACK,
    .action = NULL
};

static transition_t trans_fin_wait_1_to_closing = {
    .source = &state_fin_wait_1,
    .target = &state_closing,
    .event = TCP_EVT_FIN,
    .action = NULL
};

static transition_t trans_fin_wait_2_to_time_wait = {
    .source = &state_fin_wait_2,
    .target = &state_time_wait,
    .event = TCP_EVT_FIN,
    .action = lambda(void, (state_machine_t *sm, void *ctx), {
        printf("TCP: Sending final ACK\n");
    })
};

static transition_t trans_close_wait_to_last_ack = {
    .source = &state_close_wait,
    .target = &state_last_ack,
    .event = TCP_EVT_CLOSE,
    .action = lambda(void, (state_machine_t *sm, void *ctx), {
        printf("TCP: Sending FIN\n");
    })
};

static transition_t trans_last_ack_to_closed = {
    .source = &state_last_ack,
    .target = &state_closed,
    .event = TCP_EVT_ACK,
    .action = lambda(void, (state_machine_t *sm, void *ctx), {
        printf("TCP: Connection closed\n");
    })
};

static transition_t trans_time_wait_to_closed = {
    .source = &state_time_wait,
    .target = &state_closed,
    .event = TCP_EVT_TIMEOUT,
    .action = NULL
};

state_machine_t *tcp_state_machine_create(tcp_context_t *ctx) {
    state_machine_t *sm = sm_create("TCP", ctx);
    if (!sm) return NULL;

    /* 添加所有状态 */
    sm_add_state(sm, &state_closed);
    sm_add_state(sm, &state_listen);
    sm_add_state(sm, &state_syn_sent);
    sm_add_state(sm, &state_syn_received);
    sm_add_state(sm, &state_established);
    sm_add_state(sm, &state_fin_wait_1);
    sm_add_state(sm, &state_fin_wait_2);
    sm_add_state(sm, &state_close_wait);
    sm_add_state(sm, &state_closing);
    sm_add_state(sm, &state_last_ack);
    sm_add_state(sm, &state_time_wait);

    /* 添加所有转移 */
    sm_add_transition(sm, &trans_closed_to_syn_sent);
    sm_add_transition(sm, &trans_closed_to_listen);
    sm_add_transition(sm, &trans_listen_to_syn_received);
    sm_add_transition(sm, &trans_syn_sent_to_established);
    sm_add_transition(sm, &trans_syn_received_to_established);
    sm_add_transition(sm, &trans_established_to_fin_wait_1);
    sm_add_transition(sm, &trans_established_to_close_wait);
    sm_add_transition(sm, &trans_fin_wait_1_to_fin_wait_2);
    sm_add_transition(sm, &trans_fin_wait_1_to_closing);
    sm_add_transition(sm, &trans_fin_wait_2_to_time_wait);
    sm_add_transition(sm, &trans_close_wait_to_last_ack);
    sm_add_transition(sm, &trans_last_ack_to_closed);
    sm_add_transition(sm, &trans_time_wait_to_closed);

    sm_set_initial_state(sm, &state_closed);

    return sm;
}

/* Lambda宏定义 */
#define lambda(return_type, params, body) ({ \
    return_type lambda_func params body; \
    lambda_func; \
})
```

---

## 2. 工作流引擎 (Workflow Engine)

### 2.1 工作流核心概念

```
┌─────────────────────────────────────────────────────────────┐
│                     工作流引擎架构                           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌──────────────┐    ┌──────────────┐    ┌──────────────┐  │
│  │   流程定义    │───►│   流程引擎    │───►│   执行实例    │  │
│  │ (BPMN/DSL)   │    │ (调度/执行)  │    │ (运行时状态)  │  │
│  └──────────────┘    └──────────────┘    └──────────────┘  │
│                              │                              │
│         ┌────────────────────┼────────────────────┐         │
│         ▼                    ▼                    ▼         │
│   ┌──────────┐        ┌──────────┐        ┌──────────┐     │
│   │任务执行器 │        │ 事件监听  │        │ 持久化层  │     │
│   └──────────┘        └──────────┘        └──────────┘     │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 2.2 工作流引擎C语言实现

```c
/* workflow_engine.h */
#ifndef WORKFLOW_ENGINE_H
#define WORKFLOW_ENGINE_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

/* 任务状态 */
typedef enum {
    TASK_PENDING = 0,
    TASK_RUNNING,
    TASK_COMPLETED,
    TASK_FAILED,
    TASK_CANCELLED,
    TASK_WAITING    /* 等待外部事件 */
} task_state_t;

/* 任务类型 */
typedef enum {
    TASK_TYPE_SIMPLE = 0,   /* 简单任务 */
    TASK_TYPE_SCRIPT,       /* 脚本任务 */
    TASK_TYPE_SERVICE,      /* 服务调用 */
    TASK_TYPE_USER,         /* 用户任务 */
    TASK_TYPE_SUBPROCESS,   /* 子流程 */
    TASK_TYPE_PARALLEL,     /* 并行网关 */
    TASK_TYPE_EXCLUSIVE,    /* 排他网关 */
    TASK_TYPE_INCLUSIVE,    /* 包容网关 */
    TASK_TYPE_EVENT         /* 事件任务 */
} task_type_t;

/* 前向声明 */
typedef struct workflow_task workflow_task_t;
typedef struct workflow workflow_t;
typedef struct workflow_instance workflow_instance_t;

/* 任务处理函数 */
typedef int (*task_handler_t)(workflow_task_t *task, workflow_instance_t *inst, void *ctx);
typedef bool (*condition_evaluator_t)(workflow_task_t *task, workflow_instance_t *inst);

/* 任务定义 */
struct workflow_task {
    char id[64];
    char name[128];
    task_type_t type;
    task_state_t state;

    /* 执行配置 */
    task_handler_t handler;
    condition_evaluator_t condition;
    uint32_t retry_count;
    uint32_t retry_delay_ms;
    uint32_t timeout_ms;

    /* 连接 */
    workflow_task_t **incoming;
    uint32_t incoming_count;
    workflow_task_t **outgoing;
    uint32_t outgoing_count;

    /* 并行/网关配置 */
    uint32_t min_complete;  /* 最少完成数 */
    uint32_t max_complete;  /* 最多完成数 */

    /* 用户数据 */
    void *user_data;
};

/* 流程定义 */
struct workflow {
    char id[64];
    char name[128];
    char version[16];

    workflow_task_t **tasks;
    uint32_t task_count;
    uint32_t task_capacity;

    workflow_task_t *start_task;
    workflow_task_t **end_tasks;
    uint32_t end_task_count;

    /* 变量定义 */
    char **variable_names;
    uint32_t variable_count;
};

/* 流程实例 */
struct workflow_instance {
    char instance_id[64];
    workflow_t *workflow;

    /* 运行时状态 */
    task_state_t *task_states;
    workflow_task_t **active_tasks;
    uint32_t active_task_count;

    /* 变量存储 */
    void **variables;

    /* 执行上下文 */
    void *user_ctx;
    time_t start_time;
    time_t end_time;

    /* 持久化标记 */
    bool dirty;
    uint64_t version;  /* 乐观锁 */
};

/* 引擎配置 */
typedef struct {
    uint32_t max_concurrent_instances;
    uint32_t thread_pool_size;
    uint32_t poll_interval_ms;
    bool enable_persistence;
    const char *storage_path;
} workflow_engine_config_t;

/* 工作流引擎 */
typedef struct workflow_engine workflow_engine_t;

/* API */
workflow_engine_t *we_create(const workflow_engine_config_t *config);
void we_destroy(workflow_engine_t *engine);

/* 流程定义 */
workflow_t *we_define_workflow(workflow_engine_t *engine, const char *id, const char *name);
int we_add_task(workflow_t *wf, workflow_task_t *task);
int we_connect_tasks(workflow_t *wf, const char *from_id, const char *to_id);
int we_set_start_task(workflow_t *wf, const char *task_id);

/* 实例管理 */
workflow_instance_t *we_create_instance(workflow_engine_t *engine,
                                         const char *workflow_id,
                                         void *user_ctx);
int we_start_instance(workflow_engine_t *engine, workflow_instance_t *inst);
int we_signal_instance(workflow_engine_t *engine,
                       workflow_instance_t *inst,
                       const char *task_id,
                       void *signal_data);
int we_cancel_instance(workflow_engine_t *engine, workflow_instance_t *inst);

/* 执行 */
int we_step(workflow_engine_t *engine);
int we_run_blocking(workflow_engine_t *engine, workflow_instance_t *inst);

#endif /* WORKFLOW_ENGINE_H */
```

```c
/* workflow_engine.c */
#include "workflow_engine.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>

struct workflow_engine {
    workflow_engine_config_t config;

    /* 流程定义存储 */
    workflow_t **workflows;
    uint32_t workflow_count;
    uint32_t workflow_capacity;

    /* 实例存储 */
    workflow_instance_t **instances;
    uint32_t instance_count;
    uint32_t instance_capacity;
    pthread_mutex_t instances_lock;

    /* 执行队列 */
    workflow_instance_t **run_queue;
    uint32_t queue_head;
    uint32_t queue_tail;
    uint32_t queue_size;
    pthread_mutex_t queue_lock;
    pthread_cond_t queue_cond;

    /* 工作线程 */
    pthread_t *workers;
    bool shutdown;
};

workflow_engine_t *we_create(const workflow_engine_config_t *config) {
    workflow_engine_t *engine = calloc(1, sizeof(workflow_engine_t));
    if (!engine) return NULL;

    if (config) {
        engine->config = *config;
    } else {
        /* 默认配置 */
        engine->config.max_concurrent_instances = 1000;
        engine->config.thread_pool_size = 4;
        engine->config.poll_interval_ms = 100;
    }

    /* 初始化存储 */
    engine->workflow_capacity = 64;
    engine->workflows = calloc(engine->workflow_capacity, sizeof(workflow_t *));

    engine->instance_capacity = engine->config.max_concurrent_instances;
    engine->instances = calloc(engine->instance_capacity, sizeof(workflow_instance_t *));
    pthread_mutex_init(&engine->instances_lock, NULL);

    /* 初始化队列 */
    engine->queue_size = engine->instance_capacity;
    engine->run_queue = calloc(engine->queue_size, sizeof(workflow_instance_t *));
    pthread_mutex_init(&engine->queue_lock, NULL);
    pthread_cond_init(&engine->queue_cond, NULL);

    return engine;
}

workflow_task_t *we_create_task(const char *id, const char *name, task_type_t type) {
    workflow_task_t *task = calloc(1, sizeof(workflow_task_t));
    if (!task) return NULL;

    strncpy(task->id, id, sizeof(task->id) - 1);
    strncpy(task->name, name, sizeof(task->name) - 1);
    task->type = type;
    task->state = TASK_PENDING;
    task->retry_count = 3;
    task->timeout_ms = 30000;

    return task;
}

workflow_t *we_define_workflow(workflow_engine_t *engine, const char *id, const char *name) {
    if (!engine || !id) return NULL;

    workflow_t *wf = calloc(1, sizeof(workflow_t));
    if (!wf) return NULL;

    strncpy(wf->id, id, sizeof(wf->id) - 1);
    strncpy(wf->name, name, sizeof(wf->name) - 1);
    strncpy(wf->version, "1.0", sizeof(wf->version) - 1);

    wf->task_capacity = 64;
    wf->tasks = calloc(wf->task_capacity, sizeof(workflow_task_t *));

    /* 添加到引擎 */
    if (engine->workflow_count >= engine->workflow_capacity) {
        /* 扩容 */
    }
    engine->workflows[engine->workflow_count++] = wf;

    return wf;
}

int we_add_task(workflow_t *wf, workflow_task_t *task) {
    if (!wf || !task) return -1;

    if (wf->task_count >= wf->task_capacity) {
        size_t new_cap = wf->task_capacity * 2;
        workflow_task_t **new_tasks = realloc(wf->tasks, new_cap * sizeof(workflow_task_t *));
        if (!new_tasks) return -1;
        wf->tasks = new_tasks;
        wf->task_capacity = new_cap;
    }

    wf->tasks[wf->task_count++] = task;
    return 0;
}

/* 任务执行 */
static int execute_task(workflow_engine_t *engine, workflow_instance_t *inst,
                        workflow_task_t *task) {
    printf("[Workflow %s] Executing task: %s (%s)\n",
           inst->instance_id, task->name, task->id);

    task->state = TASK_RUNNING;
    inst->task_states[task - inst->workflow->tasks[0]] = TASK_RUNNING;

    int result = -1;

    switch (task->type) {
        case TASK_TYPE_SIMPLE:
        case TASK_TYPE_SCRIPT:
            if (task->handler) {
                result = task->handler(task, inst, inst->user_ctx);
            }
            break;

        case TASK_TYPE_PARALLEL:
            /* 启动所有出边任务 */
            for (uint32_t i = 0; i < task->outgoing_count; i++) {
                /* 添加到活跃任务 */
            }
            result = 0;
            break;

        case TASK_TYPE_EXCLUSIVE:
            /* 评估条件，选择分支 */
            for (uint32_t i = 0; i < task->outgoing_count; i++) {
                workflow_task_t *next = task->outgoing[i];
                if (!next->condition || next->condition(next, inst)) {
                    /* 选择此分支 */
                    break;
                }
            }
            result = 0;
            break;

        case TASK_TYPE_USER:
            /* 等待用户输入 */
            task->state = TASK_WAITING;
            result = 0;
            break;

        default:
            break;
    }

    if (result == 0) {
        task->state = TASK_COMPLETED;
        inst->task_states[task - inst->workflow->tasks[0]] = TASK_COMPLETED;
    } else {
        task->state = TASK_FAILED;
        inst->task_states[task - inst->workflow->tasks[0]] = TASK_FAILED;
    }

    return result;
}

/* 推进流程 */
static int advance_workflow(workflow_engine_t *engine, workflow_instance_t *inst) {
    bool progress = true;

    while (progress) {
        progress = false;

        for (uint32_t i = 0; i < inst->workflow->task_count; i++) {
            workflow_task_t *task = inst->workflow->tasks[i];
            task_state_t state = inst->task_states[i];

            if (state != TASK_PENDING && state != TASK_COMPLETED) {
                continue;
            }

            /* 检查前置条件 */
            bool ready = true;
            for (uint32_t j = 0; j < task->incoming_count; j++) {
                workflow_task_t *prev = task->incoming[j];
                int prev_idx = prev - inst->workflow->tasks[0];
                if (inst->task_states[prev_idx] != TASK_COMPLETED) {
                    ready = false;
                    break;
                }
            }

            if (ready && state == TASK_PENDING) {
                int result = execute_task(engine, inst, task);
                progress = true;

                if (result != 0) {
                    /* 处理失败 */
                    return -1;
                }
            }
        }
    }

    return 0;
}

int we_start_instance(workflow_engine_t *engine, workflow_instance_t *inst) {
    if (!engine || !inst) return -1;

    inst->start_time = time(NULL);

    /* 初始化任务状态 */
    inst->task_states = calloc(inst->workflow->task_count, sizeof(task_state_t));

    /* 从起始任务开始 */
    if (inst->workflow->start_task) {
        int idx = 0; /* 需要查找正确索引 */
        inst->task_states[idx] = TASK_PENDING;
    }

    /* 添加到运行队列 */
    pthread_mutex_lock(&engine->queue_lock);
    engine->run_queue[engine->queue_tail++ % engine->queue_size] = inst;
    pthread_cond_signal(&engine->queue_cond);
    pthread_mutex_unlock(&engine->queue_lock);

    return 0;
}
```

---

## 3. 事件驱动架构

### 3.1 事件驱动模型

```
┌─────────────────────────────────────────────────────────────┐
│                    事件驱动架构                              │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   事件源          事件总线            事件处理器              │
│     │                 │                   │                 │
│     │    事件发布      │                   │                 │
│     ├─────────────────►│                   │                 │
│     │                 │◄──────────────────┤                 │
│     │                 │    订阅/消费        │                 │
│     │                 │                   │                 │
│     │                 │─────┬─────────────┼─────┐           │
│     │                 │     │             │     │           │
│     │                 │     ▼             ▼     ▼           │
│     │                 │  Handler1     Handler2 Handler3     │
│     │                 │     │             │     │           │
│     │    新事件        │     └─────────────┴─────┘           │
│     ├────────────────►│              │                      │
│     │                 │              ▼                      │
│     │                 │          事件存储 (Event Store)       │
│     │                 │         (持久化/回放)                  │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 3.2 事件总线实现

```c
/* event_bus.h */
#ifndef EVENT_BUS_H
#define EVENT_BUS_H

#include <stdint.h>
#include <stdbool.h>

typedef uint32_t event_type_t;
typedef uint64_t event_id_t;

/* 事件结构 */
typedef struct {
    event_id_t id;
    event_type_t type;
    uint64_t timestamp;
    uint32_t source_id;
    void *payload;
    size_t payload_size;
    uint32_t priority;  /* 0-255, 数值越小优先级越高 */
} event_t;

/* 事件处理器 */
typedef void (*event_handler_t)(const event_t *event, void *user_ctx);
typedef bool (*event_filter_t)(const event_t *event, void *user_ctx);

/* 订阅 */
typedef struct {
    event_type_t type;
    event_handler_t handler;
    event_filter_t filter;
    void *user_ctx;
    uint32_t priority;
    bool async;         /* 是否异步处理 */
} subscription_t;

/* 事件总线 */
typedef struct event_bus event_bus_t;

event_bus_t *eb_create(void);
void eb_destroy(event_bus_t *bus);

/* 订阅管理 */
int eb_subscribe(event_bus_t *bus, const subscription_t *sub);
int eb_unsubscribe(event_bus_t *bus, event_type_t type, event_handler_t handler);

/* 事件发布 */
int eb_publish(event_bus_t *bus, const event_t *event);
int eb_publish_sync(event_bus_t *bus, const event_t *event);  /* 同步等待处理完成 */

/* 批处理 */
int eb_publish_batch(event_bus_t *bus, event_t *events, size_t count);

/* 查询 */
size_t eb_get_pending_count(event_bus_t *bus);
int eb_process_events(event_bus_t *bus, uint32_t max_events);

#endif /* EVENT_BUS_H */
```

---

## 4. 设计模式对比

| 模式 | 适用场景 | 复杂度 | 可扩展性 |
|------|----------|--------|----------|
| **状态机** | 明确的状态流转、协议实现 | 中 | 高 |
| **工作流** | 业务流程编排、审批流程 | 高 | 高 |
| **事件驱动** | 松耦合系统、实时处理 | 中 | 极高 |
| **规则引擎** | 复杂业务规则、决策系统 | 高 | 中 |

---

## 5. 学习资源

| 资源 | 类型 | 说明 |
|------|------|------|
| libfsm | C库 | 有限状态机实现 |
| libtask | C库 | 协程/任务调度 |
| Activiti/Java | 参考 | BPMN工作流引擎 |
| Redis事件循环 | 源码 | 高效事件驱动实现 |

---

## 总结

状态机、工作流引擎和事件驱动架构是构建复杂系统的三大支柱。状态机适合管理有明确生命周期的对象，工作流引擎擅长业务流程编排，事件驱动架构则提供了最佳的松耦合和可扩展性。在实际项目中，这三种模式往往结合使用，共同构建健壮的业务系统。
