# 函数指针状态机实现模式

> **层级定位**: 06_Thinking_Representation > 09_State_Machines
> **难度级别**: L4 (中高级)
> **前置知识**: [查表法实现](03_Table_Driven_Implementation.md), 函数指针基础
> **预估学习时间**: 2-3 小时

---

## 模块概述

函数指针实现是状态机设计的**黄金标准**。
每个状态是一个函数，状态转换通过函数调用完成。
这种模式提供了最大的灵活性，支持复杂的守卫条件、入口/出口动作，以及层次状态机。

---

## 学习目标

1. **设计状态函数接口** - 统一的状态函数签名设计
2. **实现事件分发** - 高效的事件路由机制
3. **管理状态上下文** - 状态数据的封装与传递
4. **构建层次状态机** - 状态继承与事件冒泡

---

## 一、核心设计

### 1.1 状态即函数

```c
/**
 * @file function_pointer_fsm.c
 * @brief 函数指针状态机核心实现
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* ========== 前置声明 ========== */

typedef struct FSM FSM;
typedef struct Event Event;

/* 状态函数返回的结果 */
typedef struct {
    const struct State* next_state;  /* 下一状态( NULL = 保持 ) */
    bool handled;                     /* 事件是否被处理 */
} StateResult;

/* 状态函数类型 */
typedef StateResult (*StateFunc)(FSM* fsm, const Event* evt);

/* 状态定义 */
typedef struct State {
    const char* name;
    StateFunc handler;           /* 事件处理函数 */
    void (*on_enter)(FSM* fsm);  /* 入口动作 */
    void (*on_exit)(FSM* fsm);   /* 出口动作 */
    const struct State* parent;  /* 父状态(层次状态机) */
} State;

/* 事件定义 */
typedef struct Event {
    uint32_t type;
    void* data;
    uint32_t data_len;
} Event;

/* FSM上下文 */
struct FSM {
    const State* current;
    const State* previous;  /* 用于记录历史 */
    void* user_data;        /* 用户数据 */
    uint32_t trans_count;
};
```

### 1.2 完整实现框架

```c
/* ========== 状态函数声明 ========== */

StateResult state_idle(FSM* fsm, const Event* evt);
StateResult state_running(FSM* fsm, const Event* evt);
StateResult state_paused(FSM* fsm, const Event* evt);
StateResult state_error(FSM* fsm, const Event* evt);

/* ========== 状态定义 ========== */

static const State state_idle_obj = {
    .name = "IDLE",
    .handler = state_idle,
    .on_enter = NULL,
    .on_exit = NULL,
    .parent = NULL
};

static const State state_running_obj = {
    .name = "RUNNING",
    .handler = state_running,
    .on_enter = NULL,
    .on_exit = NULL,
    .parent = NULL
};

static const State state_paused_obj = {
    .name = "PAUSED",
    .handler = state_paused,
    .on_enter = NULL,
    .on_exit = NULL,
    .parent = NULL
};

static const State state_error_obj = {
    .name = "ERROR",
    .handler = state_error,
    .on_enter = NULL,
    .on_exit = NULL,
    .parent = NULL
};

/* ========== 状态函数实现 ========== */

typedef enum {
    EVT_START = 1,
    EVT_STOP,
    EVT_PAUSE,
    EVT_RESUME,
    EVT_ERROR,
    EVT_RESET
} EventType;

StateResult state_idle(FSM* fsm, const Event* evt) {
    (void)fsm;

    switch (evt->type) {
        case EVT_START:
            return (StateResult){&state_running_obj, true};
        case EVT_ERROR:
            return (StateResult){&state_error_obj, true};
        default:
            return (StateResult){NULL, false};  /* 未处理 */
    }
}

StateResult state_running(FSM* fsm, const Event* evt) {
    (void)fsm;

    switch (evt->type) {
        case EVT_STOP:
            return (StateResult){&state_idle_obj, true};
        case EVT_PAUSE:
            return (StateResult){&state_paused_obj, true};
        case EVT_ERROR:
            return (StateResult){&state_error_obj, true};
        default:
            return (StateResult){NULL, false};
    }
}

StateResult state_paused(FSM* fsm, const Event* evt) {
    (void)fsm;

    switch (evt->type) {
        case EVT_RESUME:
            return (StateResult){&state_running_obj, true};
        case EVT_STOP:
            return (StateResult){&state_idle_obj, true};
        case EVT_ERROR:
            return (StateResult){&state_error_obj, true};
        default:
            return (StateResult){NULL, false};
    }
}

StateResult state_error(FSM* fsm, const Event* evt) {
    (void)fsm;

    switch (evt->type) {
        case EVT_RESET:
            return (StateResult){&state_idle_obj, true};
        default:
            return (StateResult){NULL, false};
    }
}

/* ========== FSM引擎 ========== */

void fsm_init(FSM* fsm, const State* initial) {
    fsm->current = initial;
    fsm->previous = NULL;
    fsm->user_data = NULL;
    fsm->trans_count = 0;

    /* 执行初始状态的入口动作 */
    if (fsm->current && fsm->current->on_enter) {
        fsm->current->on_enter(fsm);
    }
}

void fsm_dispatch(FSM* fsm, const Event* evt) {
    if (!fsm->current || !fsm->current->handler) {
        fprintf(stderr, "[FSM] 错误: 无效状态\n");
        return;
    }

    printf("[FSM] %s 处理事件 %u\n", fsm->current->name, evt->type);

    /* 调用当前状态的处理函数 */
    StateResult result = fsm->current->handler(fsm, evt);

    if (result.handled) {
        printf("  → 事件已处理\n");
    } else {
        printf("  → 事件未处理\n");
    }

    /* 状态转换 */
    if (result.next_state && result.next_state != fsm->current) {
        printf("[TRANS] %s → %s\n", fsm->current->name, result.next_state->name);

        /* 执行出口动作 */
        if (fsm->current->on_exit) {
            fsm->current->on_exit(fsm);
        }

        fsm->previous = fsm->current;
        fsm->current = result.next_state;
        fsm->trans_count++;

        /* 执行入口动作 */
        if (fsm->current->on_enter) {
            fsm->current->on_enter(fsm);
        }
    }
}

/* ========== 测试 ========== */

int main(void) {
    FSM fsm;

    printf("=== 函数指针状态机测试 ===\n\n");

    fsm_init(&fsm, &state_idle_obj);

    Event evts[] = {
        {EVT_START, NULL, 0},
        {EVT_PAUSE, NULL, 0},
        {EVT_RESUME, NULL, 0},
        {EVT_STOP, NULL, 0},
    };

    for (size_t i = 0; i < 4; i++) {
        fsm_dispatch(&fsm, &evts[i]);
        printf("\n");
    }

    printf("总转换次数: %u\n", fsm.trans_count);

    return 0;
}
```

---

## 二、高级特性

### 2.1 带守卫条件的转换

```c
/**
 * @brief 带守卫条件的状态转换
 */

typedef struct {
    uint32_t temperature;
    uint32_t pressure;
    bool emergency;
} MachineContext;

StateResult state_advanced_running(FSM* fsm, const Event* evt) {
    MachineContext* ctx = (MachineContext*)fsm->user_data;

    switch (evt->type) {
        case EVT_STOP:
            /* 守卫条件: 温度正常才能正常停止 */
            if (ctx->temperature < 80) {
                return (StateResult){&state_idle_obj, true};
            } else {
                printf("  ⚠ 温度过高，拒绝停止请求\n");
                return (StateResult){NULL, true};  /* 已处理但不转换 */
            }

        case EVT_ERROR:
            /* 自动记录错误上下文 */
            if (ctx->temperature >= 80) {
                printf("  🔥 热错误记录\n");
            }
            if (ctx->pressure >= 100) {
                printf("  💥 压力错误记录\n");
            }
            return (StateResult){&state_error_obj, true};

        default:
            return (StateResult){NULL, false};
    }
}
```

### 2.2 状态继承与事件冒泡 (层次状态机基础)

```c
/**
 * @brief 层次状态机的事件冒泡机制
 */

/* 父状态: 处理通用事件 */
StateResult state_parent_active(FSM* fsm, const Event* evt) {
    /* 所有子状态共享的处理 */
    switch (evt->type) {
        case EVT_ERROR:
            /* 任何活跃子状态遇到错误都转到错误状态 */
            return (StateResult){&state_error_obj, true};
        default:
            return (StateResult){NULL, false};  /* 让子状态处理 */
    }
}

static const State state_active_parent = {
    .name = "ACTIVE(parent)",
    .handler = state_parent_active,
    .parent = NULL
};

/* 子状态1: RUNNING */
StateResult state_running_hsm(FSM* fsm, const Event* evt) {
    switch (evt->type) {
        case EVT_PAUSE:
            return (StateResult){&state_paused_obj, true};
        case EVT_STOP:
            return (StateResult){&state_idle_obj, true};
        default:
            return (StateResult){NULL, false};  /* 冒泡到父状态 */
    }
}

static const State state_running_hsm_obj = {
    .name = "RUNNING",
    .handler = state_running_hsm,
    .parent = &state_active_parent  /* 设置父状态 */
};

/* 增强的分发函数，支持事件冒泡 */
void fsm_dispatch_hierarchical(FSM* fsm, const Event* evt) {
    const State* state = fsm->current;
    StateResult result = {NULL, false};

    /* 从当前状态开始，逐级向上冒泡 */
    while (state && !result.handled) {
        printf("[HSM] 尝试 %s\n", state->name);
        result = state->handler(fsm, evt);

        if (!result.handled) {
            state = state->parent;  /* 冒泡到父状态 */
        }
    }

    /* 执行状态转换 */
    if (result.next_state && result.next_state != fsm->current) {
        printf("[HSM-TRANS] %s → %s\n",
               fsm->current->name, result.next_state->name);
        fsm->current = result.next_state;
    }
}
```

---

## 三、完整工业案例: 网络连接管理器

```c
/**
 * @file connection_manager_fsm.c
 * @brief 函数指针实现的网络连接状态机
 * @description 管理TCP连接的完整生命周期
 */

#include <stdio.h>
#include <string.h>

/* ========== 类型定义 ========== */

typedef enum {
    CONN_DISCONNECTED,
    CONN_RESOLVING,      /* DNS解析中 */
    CONN_CONNECTING,     /* TCP连接中 */
    CONN_CONNECTED,
    CONN_AUTHENTICATING, /* 认证中 */
    CONN_READY,
    CONN_ERROR
} ConnStateType;

typedef enum {
    EVT_CONNECT_REQ,
    EVT_DNS_OK,
    EVT_DNS_FAIL,
    EVT_TCP_OK,
    EVT_TCP_FAIL,
    EVT_AUTH_OK,
    EVT_AUTH_FAIL,
    EVT_DISCONNECT_REQ,
    EVT_TIMEOUT,
    EVT_ERROR
} ConnEventType;

typedef struct {
    char host[256];
    int port;
    int retry_count;
    void* socket;
} Connection;

typedef struct {
    const char* name;
    ConnStateType type;
    StateResult (*handler)(Connection* conn, const Event* evt);
    void (*on_enter)(Connection* conn);
    void (*on_exit)(Connection* conn);
} ConnState;

/* ========== 动作函数 ========== */

void action_start_dns(Connection* conn) {
    printf("  [ACTION] 启动DNS解析: %s\n", conn->host);
    /* 启动异步DNS查询 */
}

void action_start_tcp(Connection* conn) {
    printf("  [ACTION] 启动TCP连接: %s:%d\n", conn->host, conn->port);
    /* 启动异步TCP连接 */
}

void action_start_auth(Connection* conn) {
    printf("  [ACTION] 发送认证请求\n");
    /* 发送认证包 */
}

void action_cleanup(Connection* conn) {
    printf("  [ACTION] 清理连接资源\n");
    if (conn->socket) {
        /* close(conn->socket); */
        conn->socket = NULL;
    }
    conn->retry_count = 0;
}

/* ========== 状态函数 ========== */

/* DISCONNECTED */
StateResult state_disconnected(Connection* conn, const Event* evt) {
    (void)conn;
    switch (evt->type) {
        case EVT_CONNECT_REQ:
            return (StateResult){(const State*)&state_resolving, true};
        default:
            return (StateResult){NULL, false};
    }
}

void enter_resolving(Connection* conn) {
    action_start_dns(conn);
}

/* RESOLVING */
StateResult state_resolving(Connection* conn, const Event* evt) {
    (void)conn;
    switch (evt->type) {
        case EVT_DNS_OK:
            return (StateResult){(const State*)&state_connecting, true};
        case EVT_DNS_FAIL:
        case EVT_TIMEOUT:
            return (StateResult){(const State*)&state_error, true};
        default:
            return (StateResult){NULL, false};
    }
}

void enter_connecting(Connection* conn) {
    action_start_tcp(conn);
}

/* CONNECTING */
StateResult state_connecting(Connection* conn, const Event* evt) {
    (void)conn;
    switch (evt->type) {
        case EVT_TCP_OK:
            return (StateResult){(const State*)&state_authenticating, true};
        case EVT_TCP_FAIL:
        case EVT_TIMEOUT:
            conn->retry_count++;
            if (conn->retry_count < 3) {
                return (StateResult){(const State*)&state_resolving, true};
            }
            return (StateResult){(const State*)&state_error, true};
        default:
            return (StateResult){NULL, false};
    }
}

void enter_authenticating(Connection* conn) {
    action_start_auth(conn);
}

/* AUTHENTICATING */
StateResult state_authenticating(Connection* conn, const Event* evt) {
    (void)conn;
    switch (evt->type) {
        case EVT_AUTH_OK:
            return (StateResult){(const State*)&state_ready, true};
        case EVT_AUTH_FAIL:
            return (StateResult){(const State*)&state_error, true};
        case EVT_TIMEOUT:
            return (StateResult){(const State*)&state_connected, true};
        default:
            return (StateResult){NULL, false};
    }
}

/* READY - 正常工作状态 */
StateResult state_ready(Connection* conn, const Event* evt) {
    (void)conn;
    switch (evt->type) {
        case EVT_DISCONNECT_REQ:
            return (StateResult){(const State*)&state_disconnected, true};
        case EVT_ERROR:
            return (StateResult){(const State*)&state_error, true};
        default:
            return (StateResult){NULL, false};
    }
}

/* ERROR */
StateResult state_error(Connection* conn, const Event* evt) {
    (void)conn;
    switch (evt->type) {
        case EVT_DISCONNECT_REQ:
            return (StateResult){(const State*)&state_disconnected, true};
        default:
            return (StateResult){NULL, false};
    }
}

void exit_any(Connection* conn) {
    action_cleanup(conn);
}

/* ========== 状态对象 ========== */

ConnState state_disconnected = {
    .name = "DISCONNECTED",
    .type = CONN_DISCONNECTED,
    .handler = (void*)state_disconnected,
    .on_enter = NULL,
    .on_exit = NULL
};

ConnState state_resolving = {
    .name = "RESOLVING",
    .type = CONN_RESOLVING,
    .handler = (void*)state_resolving,
    .on_enter = enter_resolving,
    .on_exit = NULL
};

ConnState state_connecting = {
    .name = "CONNECTING",
    .type = CONN_CONNECTING,
    .handler = (void*)state_connecting,
    .on_enter = enter_connecting,
    .on_exit = NULL
};

ConnState state_authenticating = {
    .name = "AUTHENTICATING",
    .type = CONN_AUTHENTICATING,
    .handler = (void*)state_authenticating,
    .on_enter = enter_authenticating,
    .on_exit = NULL
};

ConnState state_ready = {
    .name = "READY",
    .type = CONN_READY,
    .handler = (void*)state_ready,
    .on_enter = NULL,
    .on_exit = NULL
};

ConnState state_error = {
    .name = "ERROR",
    .type = CONN_ERROR,
    .handler = (void*)state_error,
    .on_enter = NULL,
    .on_exit = exit_any
};

/* ========== FSM引擎 ========== */

typedef struct {
    ConnState* current;
    Connection conn;
} ConnFSM;

void conn_fsm_init(ConnFSM* fsm) {
    fsm->current = &state_disconnected;
    memset(&fsm->conn, 0, sizeof(fsm->conn));
    printf("[CONN] 初始化，状态: %s\n", fsm->current->name);
}

void conn_fsm_dispatch(ConnFSM* fsm, ConnEventType evt) {
    Event event = {.type = evt};

    printf("[CONN] %s 处理事件 %d\n", fsm->current->name, evt);

    StateResult result = fsm->current->handler(&fsm->conn, &event);

    if (result.next_state) {
        ConnState* next = (ConnState*)result.next_state;

        /* 出口动作 */
        if (fsm->current->on_exit) {
            fsm->current->on_exit(&fsm->conn);
        }

        printf("[CONN-TRANS] %s → %s\n", fsm->current->name, next->name);
        fsm->current = next;

        /* 入口动作 */
        if (fsm->current->on_enter) {
            fsm->current->on_enter(&fsm->conn);
        }
    }
}

/* ========== 测试场景 ========== */

int main(void) {
    ConnFSM fsm;
    conn_fsm_init(&fsm);

    /* 设置连接参数 */
    strcpy(fsm.conn.host, "example.com");
    fsm.conn.port = 8080;

    printf("\n=== 测试: 正常连接流程 ===\n");
    conn_fsm_dispatch(&fsm, EVT_CONNECT_REQ);  /* → RESOLVING */
    conn_fsm_dispatch(&fsm, EVT_DNS_OK);        /* → CONNECTING */
    conn_fsm_dispatch(&fsm, EVT_TCP_OK);        /* → AUTHENTICATING */
    conn_fsm_dispatch(&fsm, EVT_AUTH_OK);       /* → READY */

    printf("\n=== 测试: 断开连接 ===\n");
    conn_fsm_dispatch(&fsm, EVT_DISCONNECT_REQ); /* → DISCONNECTED */

    printf("\n=== 测试: 连接失败重试 ===\n");
    conn_fsm_init(&fsm);
    strcpy(fsm.conn.host, "bad-host.com");
    fsm.conn.port = 8080;

    conn_fsm_dispatch(&fsm, EVT_CONNECT_REQ);  /* → RESOLVING */
    conn_fsm_dispatch(&fsm, EVT_TCP_FAIL);      /* → CONNECTING (retry) */
    conn_fsm_dispatch(&fsm, EVT_TCP_FAIL);      /* → CONNECTING (retry) */
    conn_fsm_dispatch(&fsm, EVT_TCP_FAIL);      /* → ERROR */

    return 0;
}
```

---

## 四、思维表征: 三种实现对比

```
┌─────────────────────────────────────────────────────────────┐
│              三种状态机实现方式对比矩阵                       │
├─────────────────────────────────────────────────────────────┤
│ 特性          Switch-Case   查表法        函数指针           │
├─────────────────────────────────────────────────────────────┤
│ 代码复杂度    ⭐⭐          ⭐⭐⭐        ⭐⭐⭐⭐          │
│ 运行时效率    ⭐⭐⭐⭐       ⭐⭐⭐⭐       ⭐⭐⭐⭐⭐        │
│ 可维护性      ⭐⭐          ⭐⭐⭐⭐       ⭐⭐⭐⭐⭐        │
│ 灵活性        ⭐⭐          ⭐⭐⭐         ⭐⭐⭐⭐⭐        │
│ 内存占用      ⭐⭐⭐⭐       ⭐⭐⭐⭐       ⭐⭐⭐           │
│ 调试友好度    ⭐⭐⭐         ⭐⭐⭐         ⭐⭐⭐⭐⭐        │
│ 适合状态数    <15          10-50         >20               │
│ 层次状态机    ❌           ⚠️            ✅               │
│ 守卫条件      ⚠️           ⚠️            ✅               │
│ 动态修改      ❌           ✅            ✅               │
├─────────────────────────────────────────────────────────────┤
│ 推荐使用场景                                               │
│ • Switch-Case: 简单设备、启动代码、小型嵌入式              │
│ • 查表法:     协议解析、查找密集型、需要运行时修改          │
│ • 函数指针:   复杂应用、层次状态、需要丰富动作             │
└─────────────────────────────────────────────────────────────┘
```

---

## 五、最佳实践

### 5.1 状态函数设计原则

```c
/**
 * @brief 良好设计的状态函数模板
 */

StateResult well_designed_state_handler(FSM* fsm, const Event* evt) {
    /* 1. 参数验证 */
    if (!fsm || !evt) {
        return (StateResult){NULL, false};
    }

    /* 2. 获取上下文 */
    MyContext* ctx = (MyContext*)fsm->user_data;

    /* 3. 事件分发 */
    switch (evt->type) {
        case EVT_SPECIFIC: {
            /* 4. 守卫条件检查 */
            if (!is_condition_met(ctx)) {
                return (StateResult){NULL, true};  /* 已处理但不转换 */
            }

            /* 5. 执行动作 */
            perform_action(ctx);

            /* 6. 返回转换结果 */
            return (StateResult){&target_state, true};
        }

        /* 7. 明确忽略的事件 */
        case EVT_IGNORED_1:
        case EVT_IGNORED_2:
            return (StateResult){NULL, true};  /* 已处理，不转换 */

        /* 8. 默认: 未处理(冒泡) */
        default:
            return (StateResult){NULL, false};
    }
}
```

### 5.2 内存管理策略

```c
/* 静态分配: 嵌入式首选 */
static const State states[] = {
    /* ... */
};

/* 动态工厂: 需要运行时创建 */
State* create_dynamic_state(const char* name, StateFunc handler) {
    State* s = malloc(sizeof(State));
    s->name = strdup(name);
    s->handler = handler;
    /* ... */
    return s;
}
```

---

## 六、知识检验

### 自测题

1. **函数指针实现相比Switch-Case的核心优势是什么?**

2. **如何在函数指针实现中支持层次状态机?**

3. **StateResult结构为什么要分开handled和next_state?**

4. **在什么情况下应该使用函数指针而非查表法?**

### 代码练习

**练习: 层次状态机设计**

设计一个多媒体播放器状态机：

- 父状态: STOPPED / PLAYING
- PLAYING子状态: NORMAL / FAST_FORWARD / FAST_REWIND
- 实现事件冒泡: PLAYING父状态处理PAUSE/STOP，子状态处理播放控制

---

## 七、延伸阅读

- [Switch-Case实现](02_Switch_Case_Implementation.md) - 基础实现方式
- [查表法实现](03_Table_Driven_Implementation.md) - 数据驱动的替代方案
- [层次状态机](05_Hierarchical_State_Machines.md) - 完整HSM实现

---

**最后更新**: 2026-03-24
**维护者**: C_Lang Knowledge Base Team
**质量等级**: L4 (深化)
