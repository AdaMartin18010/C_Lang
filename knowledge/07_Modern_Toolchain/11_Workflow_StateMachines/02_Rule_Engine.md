# 规则引擎与工作流


---

## 📑 目录

- [规则引擎与工作流](#规则引擎与工作流)
  - [📑 目录](#-目录)
  - [规则引擎](#规则引擎)
    - [简单规则引擎](#简单规则引擎)
    - [决策表](#决策表)
  - [BPM工作流引擎](#bpm工作流引擎)
  - [定时任务调度](#定时任务调度)
  - [参考](#参考)


---

## 规则引擎

### 简单规则引擎

```c
/* 基于条件-动作的规则引擎 */

typedef struct {
    char name[64];
    bool (*condition)(void* context);
    void (*action)(void* context);
    int priority;
    bool enabled;
} rule_t;

typedef struct {
    rule_t* rules;
    int count;
    int capacity;
} rule_engine_t;

rule_engine_t* rule_engine_create(void) {
    rule_engine_t* engine = malloc(sizeof(rule_engine_t));
    engine->capacity = 100;
    engine->rules = malloc(sizeof(rule_t) * engine->capacity);
    engine->count = 0;
    return engine;
}

void rule_add(rule_engine_t* engine, const char* name,
              bool (*cond)(void*), void (*action)(void*), int priority) {
    if (engine->count >= engine->capacity) return;

    rule_t* r = &engine->rules[engine->count++];
    strncpy(r->name, name, sizeof(r->name));
    r->condition = cond;
    r->action = action;
    r->priority = priority;
    r->enabled = true;

    /* 按优先级排序 */
    qsort(engine->rules, engine->count, sizeof(rule_t),
          (int(*)(const void*, const void*))
          lambda(int, (const rule_t* a, const rule_t* b) {
              return b->priority - a->priority;
          }));
}

void rule_engine_execute(rule_engine_t* engine, void* context) {
    for (int i = 0; i < engine->count; i++) {
        rule_t* r = &engine->rules[i];
        if (!r->enabled) continue;

        if (r->condition(context)) {
            printf("Rule fired: %s\n", r->name);
            r->action(context);

            /* 某些规则可能阻止后续规则 */
            /* if (r->stop_on_match) break; */
        }
    }
}

/* 示例: 电商促销规则 */
typedef struct {
    double cart_total;
    bool is_vip;
    int item_count;
    double discount;
} cart_context_t;

bool cond_vip_discount(void* ctx) {
    cart_context_t* c = ctx;
    return c->is_vip && c->cart_total > 100;
}

void action_vip_discount(void* ctx) {
    cart_context_t* c = ctx;
    c->discount += c->cart_total * 0.1;
    printf("VIP 10%% discount applied\n");
}

bool cond_bulk_discount(void* ctx) {
    cart_context_t* c = ctx;
    return c->item_count >= 10;
}

void action_bulk_discount(void* ctx) {
    cart_context_t* c = ctx;
    c->discount += 20;
    printf("Bulk purchase $20 off applied\n");
}

void example_rule_engine(void) {
    rule_engine_t* engine = rule_engine_create();

    rule_add(engine, "VIP Discount", cond_vip_discount,
             action_vip_discount, 100);
    rule_add(engine, "Bulk Discount", cond_bulk_discount,
             action_bulk_discount, 50);

    cart_context_t cart = {
        .cart_total = 200,
        .is_vip = true,
        .item_count = 5,
        .discount = 0
    };

    rule_engine_execute(engine, &cart);
    printf("Final discount: $%.2f\n", cart.discount);
}
```

### 决策表

```c
/* 决策表: 条件组合 -> 动作 */

#define MAX_CONDITIONS 8
#define MAX_ACTIONS 4

typedef enum { COND_FALSE, COND_TRUE, COND_DONT_CARE } cond_value_t;

typedef struct {
    cond_value_t conditions[MAX_CONDITIONS];
    int action_id;
} rule_row_t;

typedef struct {
    int condition_count;
    int action_count;
    rule_row_t* rows;
    int row_count;
} decision_table_t;

/* 评估决策表 */
int decision_table_evaluate(decision_table_t* table, bool* facts) {
    for (int i = 0; i < table->row_count; i++) {
        rule_row_t* row = &table->rows[i];
        bool match = true;

        for (int j = 0; j < table->condition_count; j++) {
            if (row->conditions[j] == COND_DONT_CARE) continue;

            bool expected = (row->conditions[j] == COND_TRUE);
            if (facts[j] != expected) {
                match = false;
                break;
            }
        }

        if (match) {
            return row->action_id;
        }
    }
    return -1;  /* 无匹配 */
}

/* 示例: 保险费率决策 */
/* 条件: [年龄>25, 有驾照, 无事故记录, 驾龄>3年] */
/* 费率: 1=高, 2=中, 3=低 */
decision_table_t insurance_table = {
    .condition_count = 4,
    .action_count = 3,
    .rows = (rule_row_t[]){
        {{COND_FALSE, COND_TRUE,  COND_DONT_CARE, COND_DONT_CARE}, 1},  /* 年轻: 高 */
        {{COND_TRUE,  COND_TRUE,  COND_FALSE,      COND_DONT_CARE}, 2},  /* 有事故: 中 */
        {{COND_TRUE,  COND_TRUE,  COND_TRUE,       COND_FALSE},      2},  /* 驾龄短: 中 */
        {{COND_TRUE,  COND_TRUE,  COND_TRUE,       COND_TRUE},       3},  /* 全部满足: 低 */
    },
    .row_count = 4
};
```

---

## BPM工作流引擎

```c
/* 简单工作流引擎 */

typedef enum {
    NODE_START,
    NODE_END,
    NODE_TASK,          /* 人工任务 */
    NODE_SERVICE,       /* 自动服务 */
    NODE_GATEWAY_EXCLUSIVE,  /* 排他网关 */
    NODE_GATEWAY_PARALLEL    /* 并行网关 */
} node_type_t;

typedef struct workflow_node {
    char id[32];
    node_type_t type;
    char name[64];

    /* 执行函数 */
    int (*execute)(struct workflow_node* node, void* context);

    /* 流转条件 */
    struct {
        char condition[256];  /* 表达式 */
        struct workflow_node* target;
    } transitions[4];
    int transition_count;
} workflow_node_t;

typedef struct {
    char id[32];
    char name[64];
    workflow_node_t* nodes;
    int node_count;
    workflow_node_t* start_node;
} workflow_definition_t;

typedef struct {
    workflow_definition_t* definition;
    workflow_node_t* current_node;
    void* context;
    int status;  /* 0=running, 1=completed, -1=error */
} workflow_instance_t;

workflow_instance_t* workflow_start(workflow_definition_t* def, void* ctx) {
    workflow_instance_t* inst = malloc(sizeof(workflow_instance_t));
    inst->definition = def;
    inst->current_node = def->start_node;
    inst->context = ctx;
    inst->status = 0;
    return inst;
}

/* 执行一步 */
int workflow_step(workflow_instance_t* inst) {
    if (inst->status != 0 || !inst->current_node) {
        return -1;
    }

    workflow_node_t* node = inst->current_node;
    printf("Executing: %s\n", node->name);

    /* 执行节点 */
    int rc = node->execute(node, inst->context);
    if (rc != 0) {
        inst->status = -1;
        return rc;
    }

    /* 选择下一个节点 */
    if (node->type == NODE_END) {
        inst->status = 1;
        inst->current_node = NULL;
        return 0;
    }

    /* 简单流转: 选择第一个满足条件的 */
    for (int i = 0; i < node->transition_count; i++) {
        /* 评估条件... */
        bool condition_met = true;

        if (condition_met) {
            inst->current_node = node->transitions[i].target;
            return 0;
        }
    }

    return -1;  /* 无法流转 */
}

/* 运行到完成 */
void workflow_run(workflow_instance_t* inst) {
    while (inst->status == 0) {
        if (workflow_step(inst) != 0) break;
    }
}

/* 示例: 请假流程 */
/* START -> 提交申请 -> [经理审批] -> [HR审批] -> END */

int task_submit(void* node, void* ctx) {
    printf("员工提交请假申请\n");
    return 0;
}

int task_manager_approve(void* node, void* ctx) {
    /* 等待人工审批... */
    printf("等待经理审批...\n");
    /* 实际应该是异步的 */
    return 0;
}

workflow_definition_t leave_workflow = {
    .name = "Leave Approval",
    .nodes = (workflow_node_t[]){
        {
            .id = "start",
            .type = NODE_START,
            .name = "Start",
            .execute = lambda(int, (void* n, void* c) { return 0; }),
            .transitions = {{.target = NULL}},  /* 设置到submit */
            .transition_count = 1
        },
        {
            .id = "submit",
            .type = NODE_TASK,
            .name = "Submit Leave Request",
            .execute = task_submit,
            .transitions = {{.target = NULL}},
            .transition_count = 1
        },
        {
            .id = "manager_approval",
            .type = NODE_TASK,
            .name = "Manager Approval",
            .execute = task_manager_approve,
            .transitions = {
                {.condition = "approved", .target = NULL},
                {.condition = "rejected", .target = NULL}
            },
            .transition_count = 2
        }
    }
};
```

---

## 定时任务调度

```c
/* Cron表达式解析和调度 */

typedef struct {
    int minute[60];      /* 0-59, -1=任意 */
    int hour[24];
    int day_of_month[31];
    int month[12];
    int day_of_week[7];
} cron_expression_t;

/* 解析Cron: "*/5 * * * *" 每5分钟 */
int cron_parse(const char* expr, cron_expression_t* cron);

/* 计算下次执行时间 */
time_t cron_next(cron_expression_t* cron, time_t from);

/* 任务定义 */
typedef struct {
    char name[64];
    cron_expression_t schedule;
    void (*job)(void* arg);
    void* arg;
    bool enabled;
} scheduled_job_t;

/* 调度器 */
typedef struct {
    scheduled_job_t* jobs;
    int job_count;
    mtx_t lock;
    cnd_t cond;
    bool running;
} scheduler_t;

void scheduler_add_job(scheduler_t* s, const char* name,
                       const char* cron_expr,
                       void (*job)(void*), void* arg) {
    mtx_lock(&s->lock);

    scheduled_job_t* j = &s->jobs[s->job_count++];
    strncpy(j->name, name, sizeof(j->name));
    cron_parse(cron_expr, &j->schedule);
    j->job = job;
    j->arg = arg;
    j->enabled = true;

    mtx_unlock(&s->lock);
    cnd_signal(&s->cond);
}

/* 调度循环 */
int scheduler_thread(void* arg) {
    scheduler_t* s = arg;

    while (s->running) {
        mtx_lock(&s->lock);

        time_t now = time(NULL);
        time_t next_run = now + 3600;  /* 默认1小时后 */
        scheduled_job_t* next_job = NULL;

        /* 找到最近要执行的任务 */
        for (int i = 0; i < s->job_count; i++) {
            if (!s->jobs[i].enabled) continue;

            time_t job_next = cron_next(&s->jobs[i].schedule, now);
            if (job_next < next_run) {
                next_run = job_next;
                next_job = &s->jobs[i];
            }
        }

        /* 等待或执行 */
        int wait_sec = next_run - time(NULL);
        if (wait_sec > 0) {
            struct timespec deadline;
            clock_gettime(CLOCK_REALTIME, &deadline);
            deadline.tv_sec += wait_sec;
            cnd_timedwait(&s->cond, &s->lock, &deadline);
        }

        mtx_unlock(&s->lock);

        /* 执行任务 */
        if (next_job && time(NULL) >= next_run) {
            printf("Running job: %s\n", next_job->name);
            next_job->job(next_job->arg);
        }
    }

    return 0;
}
```

---

## 参考

- [Drools](https://www.drools.org/)
- [Activiti BPM](https://www.activiti.org/)
- [Quartz Scheduler](http://www.quartz-scheduler.org/)


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
