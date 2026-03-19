# 状态机模式

## 目录

- [状态机模式](#状态机模式)
  - [目录](#目录)
  - [简单状态机](#简单状态机)
  - [层次状态机](#层次状态机)
  - [状态表驱动](#状态表驱动)
  - [参考](#参考)

---

## 简单状态机

```c
/* 基本状态机 */

typedef enum {
    STATE_IDLE,
    STATE_RUNNING,
    STATE_PAUSED,
    STATE_STOPPED,
    STATE_COUNT
} state_t;

typedef enum {
    EVENT_START,
    EVENT_PAUSE,
    EVENT_RESUME,
    EVENT_STOP,
    EVENT_COUNT
} event_t;

/* 状态处理函数 */
typedef void (*state_handler_t)(void* ctx);

void idle_handler(void* ctx) {
    printf("Idle... waiting for start\n");
}

void running_handler(void* ctx) {
    printf("Running... processing\n");
}

void paused_handler(void* ctx) {
    printf("Paused...\n");
}

/* 状态机 */
typedef struct {
    state_t current;
    state_handler_t handlers[STATE_COUNT];
    void* context;
} state_machine_t;

void sm_init(state_machine_t* sm) {
    sm->current = STATE_IDLE;
    sm->handlers[STATE_IDLE] = idle_handler;
    sm->handlers[STATE_RUNNING] = running_handler;
    sm->handlers[STATE_PAUSED] = paused_handler;
}

void sm_run(state_machine_t* sm) {
    sm->handlers[sm->current](sm->context);
}

/* 状态转换 */
void sm_transition(state_machine_t* sm, event_t event) {
    static const state_t transition[STATE_COUNT][EVENT_COUNT] = {
        /* IDLE */    { STATE_RUNNING, STATE_IDLE,   STATE_IDLE,   STATE_IDLE },
        /* RUNNING */ { STATE_RUNNING, STATE_PAUSED, STATE_PAUSED, STATE_STOPPED },
        /* PAUSED */  { STATE_PAUSED,  STATE_PAUSED, STATE_RUNNING, STATE_STOPPED }
    };

    state_t next = transition[sm->current][event];
    if (next != sm->current) {
        printf("Transition: %d -> %d\n", sm->current, next);
        sm->current = next;
    }
}
```

## 层次状态机

```c
/* HSM: 支持状态继承 */

typedef struct hsm_state {
    const char* name;
    struct hsm_state* parent;
    void (*entry)(void* ctx);
    void (*exit)(void* ctx);
    void (*handle)(void* ctx, int event);
} hsm_state_t;

/* 顶层状态 */
hsm_state_t state_top = {
    .name = "TOP",
    .parent = NULL,
    .entry = NULL,
    .exit = NULL,
    .handle = default_handler
};

/* 运行状态 */
void running_entry(void* ctx) { printf("Enter RUNNING\n"); }
void running_exit(void* ctx) { printf("Exit RUNNING\n"); }

hsm_state_t state_running = {
    .name = "RUNNING",
    .parent = &state_top,
    .entry = running_entry,
    .exit = running_exit,
    .handle = running_handler
};

/* 运行子状态: 处理中 */
void processing_entry(void* ctx) { printf("Enter PROCESSING\n"); }

hsm_state_t state_processing = {
    .name = "PROCESSING",
    .parent = &state_running,
    .entry = processing_entry,
    .exit = NULL,
    .handle = processing_handler
};

/* 查找事件处理器 (LCA算法) */
void hsm_dispatch(hsm_state_t** current, void* ctx, int event) {
    hsm_state_t* s = *current;

    while (s != NULL) {
        if (s->handle != NULL) {
            s->handle(ctx, event);
            return;
        }
        s = s->parent;  /* 向上查找 */
    }
}

/* 状态转换 */
void hsm_transition(hsm_state_t** current, hsm_state_t* target, void* ctx) {
    hsm_state_t* src = *current;
    hsm_state_t* dst = target;

    /* 找到最近公共祖先 */
    hsm_state_t* lca = find_lca(src, dst);

    /* 退出源状态树 */
    hsm_state_t* s = src;
    while (s != lca) {
        if (s->exit) s->exit(ctx);
        s = s->parent;
    }

    /* 记录路径 */
    hsm_state_t* path[16];
    int path_len = 0;
    s = dst;
    while (s != lca) {
        path[path_len++] = s;
        s = s->parent;
    }

    /* 进入目标状态树 (逆序) */
    for (int i = path_len - 1; i >= 0; i--) {
        if (path[i]->entry) path[i]->entry(ctx);
    }

    *current = target;
}
```

## 状态表驱动

```c
/* 表格驱动状态机 */

typedef struct {
    int current_state;
    int event;
    int next_state;
    void (*action)(void* ctx);
    int guard;  /* 条件 */
} transition_t;

/* TCP状态机示例 */
enum { CLOSED, LISTEN, SYN_SENT, SYN_RECEIVED, ESTABLISHED,
       FIN_WAIT_1, FIN_WAIT_2, TIME_WAIT };
enum { OPEN, SEND, RECEIVE, CLOSE, TIMEOUT };

void action_passive_open(void* ctx) {
    /* 绑定端口，开始监听 */
}

void action_send_syn(void* ctx) {
    /* 发送SYN包 */
}

void action_send_syn_ack(void* ctx) {
    /* 发送SYN-ACK */
}

void action_send_ack(void* ctx) {
    /* 发送ACK */
}

/* 状态转换表 */
static const transition_t tcp_fsm[] = {
    /* 当前状态     事件         下一状态        动作           */
    { CLOSED,       OPEN,        LISTEN,         action_passive_open },
    { CLOSED,       SEND,        SYN_SENT,       action_send_syn },
    { LISTEN,       RECEIVE,     SYN_RECEIVED,   action_send_syn_ack },
    { SYN_SENT,     RECEIVE,     ESTABLISHED,    action_send_ack },
    { SYN_RECEIVED, RECEIVE,     ESTABLISHED,    action_send_ack },
    { FIN_WAIT_1,   RECEIVE,     FIN_WAIT_2,     NULL },
    { FIN_WAIT_2,   RECEIVE,     TIME_WAIT,      NULL },
    { TIME_WAIT,    TIMEOUT,     CLOSED,         NULL },
    { -1, -1, -1, NULL }  /* 结束 */
};

void process_event(int* current, int event, void* ctx) {
    for (int i = 0; tcp_fsm[i].current_state != -1; i++) {
        if (tcp_fsm[i].current_state == *current &&
            tcp_fsm[i].event == event) {

            /* 执行动作 */
            if (tcp_fsm[i].action) {
                tcp_fsm[i].action(ctx);
            }

            /* 转换状态 */
            *current = tcp_fsm[i].next_state;
            printf("State: %d -> %d\n", tcp_fsm[i].current_state, *current);
            return;
        }
    }

    printf("Invalid transition: state=%d, event=%d\n", *current, event);
}
```

---

## 参考

- [QP Framework](https://www.state-machine.com/)
- [Statecharts](https://www.sciencedirect.com/science/article/pii/0167642387900359)


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
