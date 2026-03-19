# 工作流与状态机 (Workflow & State Machines)

## 概述

状态机和工作流是管理软件复杂行为的强大工具。
状态机通过定义有限的状态和转换规则来建模系统行为，而工作流引擎则协调复杂业务流程的执行。
理解这些模式对于构建可维护、可靠的软件系统至关重要。

## 目录

- [工作流与状态机 (Workflow \& State Machines)](#工作流与状态机-workflow--state-machines)
  - [概述](#概述)
  - [目录](#目录)
  - [状态机模式](#状态机模式)
    - [状态机基础概念](#状态机基础概念)
    - [状态机实现模式对比](#状态机实现模式对比)
    - [实现模式详解](#实现模式详解)
      - [模式 1: switch-case 状态机](#模式-1-switch-case-状态机)
      - [模式 2: 表驱动状态机](#模式-2-表驱动状态机)
      - [模式 3: 分层状态机 (Hierarchical State Machine)](#模式-3-分层状态机-hierarchical-state-machine)
  - [工作流引擎](#工作流引擎)
    - [工作流引擎核心概念](#工作流引擎核心概念)
    - [工作流引擎架构](#工作流引擎架构)
    - [工作流引擎实现](#工作流引擎实现)
    - [工作流模式示例](#工作流模式示例)
  - [规则引擎](#规则引擎)
    - [规则引擎概述](#规则引擎概述)
    - [规则引擎架构](#规则引擎架构)
    - [Rete 算法实现](#rete-算法实现)
    - [简单规则引擎实现](#简单规则引擎实现)
  - [事件驱动架构](#事件驱动架构)
    - [事件驱动架构概述](#事件驱动架构概述)
    - [事件驱动架构模式](#事件驱动架构模式)
    - [事件总线实现](#事件总线实现)
    - [Saga 事件协调](#saga-事件协调)
    - [事件溯源实现](#事件溯源实现)
  - [总结](#总结)

---

## 状态机模式

### 状态机基础概念

有限状态机 (Finite State Machine, FSM) 由以下要素组成：

| 要素 | 描述 | 示例 |
|-----|------|------|
| 状态 (State) | 系统在特定时刻的情况 | 空闲、运行、暂停 |
| 事件 (Event) | 触发状态转换的信号 | 启动、停止、暂停 |
| 转换 (Transition) | 从一个状态到另一个状态的变化 | 空闲→运行 |
| 动作 (Action) | 状态转换时执行的操作 | 记录日志、发送通知 |
| 守卫 (Guard) | 控制转换能否发生的条件 | 权限检查、参数验证 |

### 状态机实现模式对比

| 实现方式 | 优点 | 缺点 | 适用场景 |
|---------|------|------|---------|
| switch-case | 简单直接 | 难以维护，扩展性差 | 简单状态机 |
| 状态表 | 清晰，易于修改 | 需要额外查找 | 中等复杂度 |
| 面向对象 | 封装性好 | 类爆炸 | 复杂状态机 |
| 函数指针表 | 高性能，灵活 | 类型不安全 | 性能敏感 |
| 状态机框架 | 功能完整 | 学习成本 | 企业级应用 |

```text
状态机类型:

┌─────────────────────────────────────────────────────────────┐
│                                                             │
│   有限状态机 (FSM)                                           │
│        │                                                    │
│   ┌────┴────┬────────────────┐                              │
│   │         │                │                              │
│   ▼         ▼                ▼                              │
│  Mealy    Moore           层次状态机                         │
│  米利型   摩尔型          (Hierarchical)                     │
│  输出依赖  输出仅依赖      支持子状态机                        │
│  输入和状态 当前状态                                         │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 实现模式详解

#### 模式 1: switch-case 状态机

```c
// 简单的 switch-case 状态机
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

void fsm_handle_event(state_t* current, event_t event) {
    switch (*current) {
        case STATE_IDLE:
            if (event == EVENT_START) {
                *current = STATE_RUNNING;
            }
            break;
        case STATE_RUNNING:
            if (event == EVENT_PAUSE) {
                *current = STATE_PAUSED;
            } else if (event == EVENT_STOP) {
                *current = STATE_STOPPED;
            }
            break;
        case STATE_PAUSED:
            if (event == EVENT_RESUME) {
                *current = STATE_RUNNING;
            } else if (event == EVENT_STOP) {
                *current = STATE_STOPPED;
            }
            break;
        case STATE_STOPPED:
            if (event == EVENT_START) {
                *current = STATE_RUNNING;
            }
            break;
    }
}
```

#### 模式 2: 表驱动状态机

```c
// 表驱动状态机 - 将转换逻辑分离到表中
typedef void (*action_fn_t)(void* context);
typedef bool (*guard_fn_t)(void* context);

typedef struct {
    state_t from_state;
    event_t event;
    state_t to_state;
    guard_fn_t guard;
    action_fn_t action;
    const char* description;
} transition_t;

static const transition_t transitions[] = {
    {STATE_IDLE,    EVENT_START,  STATE_RUNNING, NULL,       on_start,   "开始运行"},
    {STATE_RUNNING, EVENT_PAUSE,  STATE_PAUSED,  can_pause,  on_pause,   "暂停"},
    {STATE_RUNNING, EVENT_STOP,   STATE_STOPPED, NULL,       on_stop,    "停止"},
    {STATE_PAUSED,  EVENT_RESUME, STATE_RUNNING, NULL,       on_resume,  "恢复"},
    {STATE_PAUSED,  EVENT_STOP,   STATE_STOPPED, NULL,       on_stop,    "从暂停停止"},
    {STATE_STOPPED, EVENT_START,  STATE_RUNNING, can_restart, on_restart, "重新启动"},
};

const transition_t* find_transition(state_t from, event_t event) {
    for (size_t i = 0; i < sizeof(transitions)/sizeof(transition_t); i++) {
        if (transitions[i].from_state == from &&
            transitions[i].event == event) {
            return &transitions[i];
        }
    }
    return NULL;
}
```

#### 模式 3: 分层状态机 (Hierarchical State Machine)

```c
// 分层状态机 - 支持状态嵌套和继承
typedef struct hsm_state {
    const char* name;
    struct hsm_state* parent;
    void (*on_entry)(void* context);
    void (*on_exit)(void* context);
    int (*handle)(void* context, int event);
} hsm_state_t;

typedef struct {
    hsm_state_t* current;
    void* context;
} hsm_t;

// 事件分发 - 支持事件冒泡
int hsm_dispatch(hsm_t* hsm, int event) {
    for (hsm_state_t* s = hsm->current; s; s = s->parent) {
        if (s->handle) {
            int result = s->handle(hsm->context, event);
            if (result == 0) return 0;  // 已处理
        }
    }
    return -1;  // 未处理
}
```

---

## 工作流引擎

### 工作流引擎核心概念

工作流引擎负责协调和管理业务流程的执行：

| 组件 | 功能 | 示例 |
|-----|------|------|
| 流程定义 | 描述业务步骤和流转规则 | BPMN 图、流程配置 |
| 任务 | 流程中的单个工作单元 | 审批、数据处理 |
| 网关 | 控制流程分支和合并 | 并行网关、排他网关 |
| 事件 | 触发流程流转的信号 | 消息事件、定时事件 |
| 参与者 | 执行任务的实体 | 用户、系统服务 |

### 工作流引擎架构

```text
┌─────────────────────────────────────────────────────────────┐
│                    工作流引擎架构                            │
└─────────────────────────────────────────────────────────────┘
                              │
        ┌─────────────────────┼─────────────────────┐
        │                     │                     │
        ▼                     ▼                     ▼
┌───────────────┐     ┌───────────────┐     ┌───────────────┐
│   流程定义层   │     │   执行引擎层   │     │   运行时层    │
├───────────────┤     ├───────────────┤     ├───────────────┤
│ - 流程建模    │     │ - 流程实例化  │     │ - 任务管理    │
│ - 版本控制    │     │ - 状态机驱动  │     │ - 定时调度    │
│ - 验证校验    │     │ - 事务管理    │     │ - 事件监听    │
└───────────────┘     └───────────────┘     └───────────────┘
        │                     │                     │
        └─────────────────────┼─────────────────────┘
                              │
                              ▼
                    ┌─────────────────┐
                    │   持久化层      │
                    │ - 流程定义存储  │
                    │ - 实例状态存储  │
                    │ - 历史数据存储  │
                    └─────────────────┘
```

```text
┌─────────────────────────────────────────────────────────────┐
│                     工作流引擎架构组件                        │
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

### 工作流引擎实现

```c
// 工作流引擎核心数据结构

// 流程定义
typedef struct {
    char id[64];
    char name[256];
    int version;
    wf_node_t* nodes;
    size_t node_count;
    wf_transition_t* transitions;
    size_t transition_count;
} workflow_definition_t;

// 节点类型
typedef enum {
    NODE_START,      // 开始节点
    NODE_END,        // 结束节点
    NODE_TASK,       // 任务节点
    NODE_GATEWAY,    // 网关节点
    NODE_EVENT,      // 事件节点
    NODE_SUBPROCESS  // 子流程
} node_type_t;

// 工作流节点
typedef struct {
    char id[64];
    node_type_t type;
    char name[256];
    void (*execute)(wf_instance_t* inst, wf_node_t* node);
    // 任务特定配置
    union {
        struct {
            char assignee[128];
            char candidate_group[128];
            int timeout_seconds;
        } task;
        struct {
            int gateway_type;  // 排他/并行/包含
        } gateway;
    } config;
} wf_node_t;

// 流程实例
typedef struct {
    char id[64];
    workflow_definition_t* definition;
    wf_instance_state_t state;
    wf_node_t* current_nodes[16];  // 当前活动节点 (支持并行)
    size_t current_node_count;
    hash_table_t* variables;        // 流程变量
    time_t start_time;
    time_t end_time;
} wf_instance_t;

// 创建工作流实例
wf_instance_t* workflow_create_instance(workflow_definition_t* def) {
    wf_instance_t* inst = calloc(1, sizeof(wf_instance_t));
    uuid_generate(inst->id);
    inst->definition = def;
    inst->state = WF_STATE_ACTIVE;
    inst->variables = hash_table_create();
    inst->start_time = time(NULL);

    // 找到开始节点
    for (size_t i = 0; i < def->node_count; i++) {
        if (def->nodes[i].type == NODE_START) {
            workflow_enter_node(inst, &def->nodes[i]);
            break;
        }
    }

    return inst;
}

// 进入节点
void workflow_enter_node(wf_instance_t* inst, wf_node_t* node) {
    printf("Entering node: %s (%s)\n", node->name, node->id);

    // 添加到当前活动节点
    inst->current_nodes[inst->current_node_count++] = node;

    // 执行节点
    if (node->execute) {
        node->execute(inst, node);
    }

    // 自动流转 (非等待节点)
    if (node->type != NODE_TASK && node->type != NODE_EVENT) {
        workflow_leave_node(inst, node);
    }
}

// 离开节点
void workflow_leave_node(wf_instance_t* inst, wf_node_t* node) {
    // 从当前节点移除
    for (size_t i = 0; i < inst->current_node_count; i++) {
        if (inst->current_nodes[i] == node) {
            inst->current_nodes[i] =
                inst->current_nodes[--inst->current_node_count];
            break;
        }
    }

    // 查找并执行后续转换
    for (size_t i = 0; i < inst->definition->transition_count; i++) {
        wf_transition_t* trans = &inst->definition->transitions[i];
        if (strcmp(trans->from_node_id, node->id) == 0) {
            // 检查条件
            if (trans->condition == NULL ||
                evaluate_condition(inst, trans->condition)) {
                wf_node_t* next = find_node(inst->definition, trans->to_node_id);
                if (next) {
                    workflow_enter_node(inst, next);
                }
            }
        }
    }

    // 检查是否结束
    if (inst->current_node_count == 0) {
        inst->state = WF_STATE_COMPLETED;
        inst->end_time = time(NULL);
    }
}

// 完成任务
void workflow_complete_task(wf_instance_t* inst, const char* node_id,
                            const char* user_id) {
    wf_node_t* node = find_node(inst->definition, node_id);
    if (!node || node->type != NODE_TASK) return;

    // 验证权限
    if (!can_complete_task(inst, node, user_id)) {
        printf("Permission denied for user: %s\n", user_id);
        return;
    }

    printf("Task completed by: %s\n", user_id);
    workflow_leave_node(inst, node);
}

// 任务节点执行示例
void task_node_execute(wf_instance_t* inst, wf_node_t* node) {
    printf("Creating task: %s\n", node->name);
    printf("  Assignee: %s\n", node->config.task.assignee);
    printf("  Timeout: %d seconds\n", node->config.task.timeout_seconds);

    // 创建任务记录
    task_record_t* task = create_task(inst, node);

    // 发送通知
    if (strlen(node->config.task.assignee) > 0) {
        notify_user(node->config.task.assignee, task);
    }
}

// 网关节点执行示例
void gateway_node_execute(wf_instance_t* inst, wf_node_t* node) {
    switch (node->config.gateway.gateway_type) {
        case GATEWAY_EXCLUSIVE:
            // 排他网关：选择第一个满足条件的分支
            execute_exclusive_gateway(inst, node);
            break;
        case GATEWAY_PARALLEL:
            // 并行网关：同时激活所有分支
            execute_parallel_gateway(inst, node);
            break;
        case GATEWAY_INCLUSIVE:
            // 包含网关：激活所有满足条件的分支
            execute_inclusive_gateway(inst, node);
            break;
    }
}

// 并行网关实现
void execute_parallel_gateway(wf_instance_t* inst, wf_node_t* node) {
    // 找到所有出边
    wf_transition_t* outgoing[16];
    size_t outgoing_count = 0;

    for (size_t i = 0; i < inst->definition->transition_count; i++) {
        if (strcmp(inst->definition->transitions[i].from_node_id,
                   node->id) == 0) {
            outgoing[outgoing_count++] = &inst->definition->transitions[i];
        }
    }

    // 创建同步令牌
    sync_token_t* token = create_sync_token(inst, node->id, outgoing_count);

    // 同时激活所有分支
    for (size_t i = 0; i < outgoing_count; i++) {
        wf_node_t* next = find_node(inst->definition,
                                    outgoing[i]->to_node_id);
        if (next) {
            // 将令牌传递给子分支
            set_variable(inst, "sync_token", token);
            workflow_enter_node(inst, next);
        }
    }
}
```

### 工作流模式示例

```c
// 审批流程定义示例
workflow_definition_t* create_approval_workflow() {
    workflow_definition_t* def = calloc(1, sizeof(workflow_definition_t));
    strcpy(def->id, "approval_flow_v1");
    strcpy(def->name, "费用报销审批流程");
    def->version = 1;

    // 定义节点
    def->nodes = calloc(6, sizeof(wf_node_t));
    def->node_count = 6;

    // 开始节点
    def->nodes[0] = (wf_node_t){
        .id = "start",
        .type = NODE_START,
        .name = "开始",
        .execute = start_node_execute
    };

    // 提交申请
    def->nodes[1] = (wf_node_t){
        .id = "submit",
        .type = NODE_TASK,
        .name = "提交报销申请",
        .execute = task_node_execute,
        .config.task.assignee = "#{applicant}",
        .config.task.timeout_seconds = 86400
    };

    // 经理审批网关
    def->nodes[2] = (wf_node_t){
        .id = "manager_gateway",
        .type = NODE_GATEWAY,
        .name = "经理审批",
        .execute = gateway_node_execute,
        .config.gateway.gateway_type = GATEWAY_EXCLUSIVE
    };

    // 财务审批
    def->nodes[3] = (wf_node_t){
        .id = "finance_approve",
        .type = NODE_TASK,
        .name = "财务审批",
        .execute = task_node_execute,
        .config.task.candidate_group = "finance_dept"
    };

    // 支付处理
    def->nodes[4] = (wf_node_t){
        .id = "payment",
        .type = NODE_TASK,
        .name = "支付处理",
        .execute = task_node_execute,
        .config.task.candidate_group = "payment_system"
    };

    // 结束节点
    def->nodes[5] = (wf_node_t){
        .id = "end",
        .type = NODE_END,
        .name = "流程结束",
        .execute = end_node_execute
    };

    // 定义转换
    def->transitions = calloc(5, sizeof(wf_transition_t));
    def->transition_count = 5;

    def->transitions[0] = (wf_transition_t){"start", "submit", NULL};
    def->transitions[1] = (wf_transition_t){"submit", "manager_gateway", NULL};
    def->transitions[2] = (wf_transition_t){"manager_gateway", "finance_approve",
                                            "#{amount} < 10000"};
    def->transitions[3] = (wf_transition_t){"finance_approve", "payment", NULL};
    def->transitions[4] = (wf_transition_t){"payment", "end", NULL};

    return def;
}
```

---

## 规则引擎

### 规则引擎概述

规则引擎将业务规则从应用程序代码中分离出来，使得业务规则可以独立管理和修改。

### 规则引擎架构

| 组件 | 职责 | 实现方式 |
|-----|------|---------|
| 规则定义 | 描述业务规则 | DSL、配置文件 |
| 事实对象 | 规则操作的数据 | 数据结构、对象 |
| 模式匹配 | 识别满足条件的规则 | Rete 算法、线性扫描 |
| 执行引擎 | 执行匹配的规则 | 顺序执行、优先级调度 |
| 工作内存 | 存储事实对象 | 哈希表、对象池 |

### Rete 算法实现

```c
// Rete 网络节点类型
typedef enum {
    RETE_ROOT,           // 根节点
    RETE_TYPE,           // 类型节点
    RETE_ALPHA,          // Alpha 节点 (单条件测试)
    RETE_BETA,           // Beta 节点 (连接节点)
    RETE_TERMINAL        // 终端节点 (规则)
} rete_node_type_t;

// Rete 网络节点
typedef struct rete_node {
    rete_node_type_t type;
    char name[128];

    // Alpha 节点：条件测试
    struct {
        char field[64];
        int op;           // EQ, GT, LT, NE, etc.
        value_t value;
    } alpha_test;

    // Beta 节点：连接条件
    struct {
        struct rete_node* left_input;
        struct rete_node* right_input;
        join_constraint_t* constraints;
    } beta_join;

    // 子节点
    struct rete_node** children;
    size_t child_count;

    // 记忆
    token_list_t* tokens;  // Beta 记忆
    fact_list_t* facts;    // Alpha 记忆
} rete_node_t;

// 事实
typedef struct {
    char type[64];
    char id[64];
    hash_table_t* fields;
    time_t timestamp;
} fact_t;

// 规则
typedef struct {
    char name[128];
    int salience;           // 优先级
    condition_t* conditions;
    size_t condition_count;
    action_t* actions;
    size_t action_count;
    rete_node_t* terminal_node;
} rule_t;

// 创建 Rete 网络
rete_network_t* rete_create(void) {
    rete_network_t* net = calloc(1, sizeof(rete_network_t));
    net->root = calloc(1, sizeof(rete_node_t));
    net->root->type = RETE_ROOT;
    return net;
}

// 添加规则到网络
void rete_add_rule(rete_network_t* net, rule_t* rule) {
    rete_node_t* current = net->root;

    // 为每个条件创建/复用 Alpha 节点
    for (size_t i = 0; i < rule->condition_count; i++) {
        condition_t* cond = &rule->conditions[i];

        // 查找或创建类型节点
        rete_node_t* type_node = find_or_create_type_node(current, cond->fact_type);

        // 查找或创建 Alpha 节点
        rete_node_t* alpha_node = find_or_create_alpha_node(type_node, cond);

        // 创建 Beta 节点 (除第一个条件外)
        if (i > 0) {
            rete_node_t* beta_node = create_beta_node(current, alpha_node, cond);
            current = beta_node;
        } else {
            current = alpha_node;
        }
    }

    // 创建终端节点
    rete_node_t* terminal = create_terminal_node(current, rule);
    rule->terminal_node = terminal;
}

// 插入事实
void rete_assert_fact(rete_network_t* net, fact_t* fact) {
    // 找到对应的类型节点
    rete_node_t* type_node = find_type_node(net->root, fact->type);
    if (!type_node) return;

    // 传播到所有子 Alpha 节点
    for (size_t i = 0; i < type_node->child_count; i++) {
        rete_node_t* alpha = type_node->children[i];

        // 测试 Alpha 条件
        if (evaluate_alpha_test(alpha, fact)) {
            // 添加到 Alpha 记忆
            add_to_alpha_memory(alpha, fact);

            // 传播到子节点
            propagate_to_children(alpha, fact);
        }
    }
}

// Beta 节点连接
void beta_join(rete_node_t* beta_node, token_t* left_token, fact_t* right_fact) {
    // 测试连接约束
    for (size_t i = 0; i < beta_node->beta_join.constraint_count; i++) {
        if (!evaluate_join_constraint(&beta_node->beta_join.constraints[i],
                                       left_token, right_fact)) {
            return;
        }
    }

    // 创建新 token
    token_t* new_token = create_token(left_token, right_fact);
    add_to_beta_memory(beta_node, new_token);

    // 传播到子节点
    for (size_t i = 0; i < beta_node->child_count; i++) {
        rete_node_t* child = beta_node->children[i];
        if (child->type == RETE_TERMINAL) {
            // 触发规则
            activate_rule(child->rule, new_token);
        } else if (child->type == RETE_BETA) {
            // 继续连接
            for (size_t j = 0; j < child->beta_join.right_input->fact_count; j++) {
                beta_join(child, new_token, child->beta_join.right_input->facts[j]);
            }
        }
    }
}
```

### 简单规则引擎实现

```c
// 简化版规则引擎 - 适合中小规模应用

typedef bool (*condition_fn_t)(fact_t* facts, size_t fact_count);
typedef void (*action_fn_t)(fact_t* facts, size_t fact_count, void* context);

typedef struct {
    char name[128];
    int priority;
    condition_fn_t when;
    action_fn_t then;
} simple_rule_t;

typedef struct {
    simple_rule_t* rules;
    size_t rule_count;
    fact_t* facts;
    size_t fact_count;
    void* context;
} simple_engine_t;

// 执行规则引擎
void engine_execute(simple_engine_t* engine) {
    bool any_fired = true;

    while (any_fired) {
        any_fired = false;

        // 按优先级排序评估规则
        for (size_t i = 0; i < engine->rule_count; i++) {
            simple_rule_t* rule = &engine->rules[i];

            if (rule->when(engine->facts, engine->fact_count)) {
                printf("Rule fired: %s\n", rule->name);
                rule->then(engine->facts, engine->fact_count, engine->context);
                any_fired = true;

                // 重新评估 (事实可能已改变)
                break;
            }
        }
    }
}

// 示例：折扣规则
bool discount_condition(fact_t* facts, size_t count) {
    // 查找订单事实
    fact_t* order = find_fact_by_type(facts, count, "Order");
    if (!order) return false;

    double amount = get_fact_double(order, "amount");
    return amount > 1000;
}

void discount_action(fact_t* facts, size_t count, void* context) {
    fact_t* order = find_fact_by_type(facts, count, "Order");
    double amount = get_fact_double(order, "amount");
    double discount = amount * 0.1;  // 10% 折扣

    set_fact_double(order, "discount", discount);
    set_fact_double(order, "final_amount", amount - discount);

    printf("Applied 10%% discount: %.2f\n", discount);
}
```

---

## 事件驱动架构

### 事件驱动架构概述

事件驱动架构 (EDA) 通过事件的发布和订阅来实现组件间的松耦合通信。

### 事件驱动架构模式

| 模式 | 描述 | 适用场景 |
|-----|------|---------|
| 发布-订阅 | 多个订阅者接收同一事件 | 广播通知 |
| 事件溯源 | 存储事件序列作为真相源 | 审计、回滚 |
| CQRS | 读写分离，读模型基于事件构建 | 高读性能 |
| Saga | 通过事件协调分布式事务 | 微服务事务 |
| 事件总线 | 中央事件路由 | 企业集成 |

```text
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

### 事件总线实现

```c
// 事件总线核心组件

typedef struct event {
    char type[128];
    char id[64];
    void* payload;
    size_t payload_size;
    time_t timestamp;
    char source[128];
} event_t;

typedef void (*event_handler_t)(event_t* event, void* context);

typedef struct {
    char event_type[128];
    event_handler_t handler;
    void* context;
    int priority;
} subscription_t;

typedef struct {
    subscription_t* subscriptions;
    size_t subscription_count;
    pthread_mutex_t lock;

    // 异步处理
    queue_t* event_queue;
    pthread_t* worker_threads;
    size_t worker_count;
    bool running;
} event_bus_t;

// 创建事件总线
event_bus_t* event_bus_create(size_t worker_count) {
    event_bus_t* bus = calloc(1, sizeof(event_bus_t));
    bus->subscriptions = calloc(64, sizeof(subscription_t));
    pthread_mutex_init(&bus->lock, NULL);

    // 创建事件队列
    bus->event_queue = queue_create(1024);
    bus->worker_count = worker_count;
    bus->worker_threads = calloc(worker_count, sizeof(pthread_t));
    bus->running = true;

    // 启动工作线程
    for (size_t i = 0; i < worker_count; i++) {
        pthread_create(&bus->worker_threads[i], NULL,
                       event_worker_thread, bus);
    }

    return bus;
}

// 订阅事件
void event_bus_subscribe(event_bus_t* bus, const char* event_type,
                         event_handler_t handler, void* context,
                         int priority) {
    pthread_mutex_lock(&bus->lock);

    subscription_t* sub = &bus->subscriptions[bus->subscription_count++];
    strncpy(sub->event_type, event_type, 127);
    sub->handler = handler;
    sub->context = context;
    sub->priority = priority;

    pthread_mutex_unlock(&bus->lock);
}

// 发布事件
void event_bus_publish(event_bus_t* bus, event_t* event) {
    // 复制事件到队列
    event_t* event_copy = copy_event(event);
    queue_enqueue(bus->event_queue, event_copy);
}

// 工作线程
void* event_worker_thread(void* arg) {
    event_bus_t* bus = (event_bus_t*)arg;

    while (bus->running) {
        event_t* event = queue_dequeue_timeout(bus->event_queue, 100);
        if (!event) continue;

        // 查找匹配的订阅者
        pthread_mutex_lock(&bus->lock);

        // 按优先级排序处理
        subscription_t matching[64];
        size_t match_count = 0;

        for (size_t i = 0; i < bus->subscription_count; i++) {
            if (strcmp(bus->subscriptions[i].event_type, event->type) == 0 ||
                strcmp(bus->subscriptions[i].event_type, "*") == 0) {
                matching[match_count++] = bus->subscriptions[i];
            }
        }

        pthread_mutex_unlock(&bus->lock);

        // 按优先级排序
        qsort(matching, match_count, sizeof(subscription_t),
              compare_subscription_priority);

        // 调用处理器
        for (size_t i = 0; i < match_count; i++) {
            matching[i].handler(event, matching[i].context);
        }

        free_event(event);
    }

    return NULL;
}
```

### Saga 事件协调

```c
// 使用事件协调的 Saga 实现

typedef enum {
    SAGA_EVENT_STARTED,
    SAGA_EVENT_STEP_COMPLETED,
    SAGA_EVENT_STEP_FAILED,
    SAGA_EVENT_COMPENSATION_COMPLETED,
    SAGA_EVENT_COMPLETED
} saga_event_type_t;

typedef struct {
    saga_event_type_t type;
    char saga_id[64];
    int step_index;
    char error_message[256];
} saga_event_t;

// Saga 协调器
typedef struct {
    char id[64];
    saga_definition_t* definition;
    saga_status_t status;
    int current_step;
    event_bus_t* event_bus;
} saga_orchestrator_t;

// 启动 Saga
void saga_start(saga_orchestrator_t* orchestrator) {
    saga_event_t event = {
        .type = SAGA_EVENT_STARTED,
        .saga_id = orchestrator->id,
        .step_index = 0
    };
    event_bus_publish(orchestrator->event_bus, &event);

    // 执行第一步
    saga_execute_step(orchestrator, 0);
}

// 执行步骤
void saga_execute_step(saga_orchestrator_t* orchestrator, int step_index) {
    saga_step_t* step = &orchestrator->definition->steps[step_index];

    // 发送执行命令事件
    command_event_t cmd = {
        .type = step->action_command,
        .saga_id = orchestrator->id,
        .step_index = step_index,
        .payload = step->context
    };
    event_bus_publish(orchestrator->event_bus, &cmd);
}

// Saga 事件处理器
void saga_event_handler(event_t* event, void* context) {
    saga_orchestrator_t* orchestrator = (saga_orchestrator_t*)context;
    saga_event_t* saga_event = (saga_event_t*)event->payload;

    switch (saga_event->type) {
        case SAGA_EVENT_STEP_COMPLETED:
            if (saga_event->step_index == orchestrator->current_step) {
                orchestrator->current_step++;

                if (orchestrator->current_step < orchestrator->definition->step_count) {
                    // 执行下一步
                    saga_execute_step(orchestrator, orchestrator->current_step);
                } else {
                    // Saga 完成
                    saga_event_t completed = {
                        .type = SAGA_EVENT_COMPLETED,
                        .saga_id = orchestrator->id
                    };
                    event_bus_publish(orchestrator->event_bus, &completed);
                    orchestrator->status = SAGA_COMPLETED;
                }
            }
            break;

        case SAGA_EVENT_STEP_FAILED:
            // 开始补偿
            orchestrator->status = SAGA_COMPENSATING;
            saga_start_compensation(orchestrator, saga_event->step_index);
            break;

        case SAGA_EVENT_COMPENSATION_COMPLETED:
            if (orchestrator->compensation_index >= 0) {
                saga_continue_compensation(orchestrator);
            } else {
                orchestrator->status = SAGA_FAILED;
            }
            break;
    }
}
```

### 事件溯源实现

```c
// 事件溯源存储

typedef struct {
    char aggregate_id[64];
    char aggregate_type[64];
    int version;
    event_t* events;
    size_t event_count;
} event_stream_t;

typedef struct {
    // 事件存储
    int (*append)(const char* stream_id, event_t* events, size_t count);
    event_stream_t* (*read)(const char* stream_id, int from_version);

    // 快照
    int (*save_snapshot)(const char* stream_id, void* snapshot, int version);
    void* (*load_snapshot)(const char* stream_id, int* version);
} event_store_t;

// 聚合根
typedef struct {
    char id[64];
    int version;
    void* state;
    event_t* uncommitted_events;
    size_t uncommitted_count;
} aggregate_root_t;

// 应用事件到聚合
void aggregate_apply(aggregate_root_t* aggregate, event_t* event);

// 加载聚合
aggregate_root_t* aggregate_load(event_store_t* store, const char* id,
                                  void (*apply_fn)(void*, event_t*)) {
    // 尝试加载快照
    int snapshot_version = 0;
    void* snapshot = store->load_snapshot(id, &snapshot_version);

    aggregate_root_t* aggregate = calloc(1, sizeof(aggregate_root_t));
    strcpy(aggregate->id, id);
    aggregate->version = snapshot_version;
    aggregate->state = snapshot ? snapshot : create_initial_state();

    // 加载快照后的事件
    event_stream_t* stream = store->read(id, snapshot_version + 1);

    // 重放事件
    for (size_t i = 0; i < stream->event_count; i++) {
        apply_fn(aggregate->state, &stream->events[i]);
        aggregate->version++;
    }

    return aggregate;
}

// 保存聚合
void aggregate_save(event_store_t* store, aggregate_root_t* aggregate) {
    if (aggregate->uncommitted_count == 0) return;

    // 乐观并发控制
    for (size_t i = 0; i < aggregate->uncommitted_count; i++) {
        aggregate->uncommitted_events[i].expected_version = aggregate->version + i;
    }

    // 追加到事件存储
    int result = store->append(aggregate->id, aggregate->uncommitted_events,
                               aggregate->uncommitted_count);

    if (result == CONCURRENCY_CONFLICT) {
        // 处理冲突
        handle_concurrency_conflict(aggregate);
    } else {
        aggregate->version += aggregate->uncommitted_count;
        aggregate->uncommitted_count = 0;

        // 可选：创建快照
        if (aggregate->version % SNAPSHOT_FREQUENCY == 0) {
            store->save_snapshot(aggregate->id, aggregate->state, aggregate->version);
        }
    }
}
```

---

## 总结

状态机和工作流模式为复杂业务逻辑提供了结构化的解决方案：

1. **状态机模式** 适用于具有明确状态和转换的系统
2. **工作流引擎** 适合协调多步骤业务流程
3. **规则引擎** 将业务规则与应用程序分离
4. **事件驱动架构** 实现组件间的松耦合

选择合适的模式取决于业务复杂度、性能要求和团队熟悉度。


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
