/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Industrial_Linux\01_Industrial_Linux_System.md
 * Line: 4450
 * Language: c
 * Block ID: 22a7e7a9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file rule_engine.c
 * @brief 边缘规则引擎实现
 * @details 轻量级规则引擎，支持条件判断、动作执行和告警触发
 *
 * 编译: gcc -o rule_engine rule_engine.c -lm
 * 运行: ./rule_engine
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#define MAX_RULES           64
#define MAX_CONDITIONS      8
#define MAX_ACTIONS         4
#define MAX_TAG_NAME        32
#define MAX_EXPRESSION      256

/* 数据类型 */
typedef enum {
    TYPE_BOOL = 0,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING
} value_type_t;

/* 数据值 */
typedef struct {
    value_type_t type;
    union {
        bool b;
        int64_t i;
        double f;
        char s[64];
    } data;
    uint64_t timestamp;
    uint8_t quality;
} data_value_t;

/* 测点 */
typedef struct {
    char name[MAX_TAG_NAME];
    data_value_t value;
    void (*on_change)(struct tag *);
} tag_t;

/* 比较操作符 */
typedef enum {
    OP_EQ = 0,      /* == */
    OP_NE,          /* != */
    OP_GT,          /* > */
    OP_GE,          /* >= */
    OP_LT,          /* < */
    OP_LE,          /* <= */
    OP_IN_RANGE,    /* within range */
    OP_OUT_RANGE    /* outside range */
} operator_t;

/* 条件 */
typedef struct {
    char tag_name[MAX_TAG_NAME];
    operator_t op;
    data_value_t threshold;
    data_value_t threshold2;    /* 用于范围比较 */
    uint32_t duration_ms;       /* 持续时间要求 */
    uint64_t trigger_time;      /* 首次触发时间 */
    bool last_result;
} condition_t;

/* 动作类型 */
typedef enum {
    ACTION_LOG = 0,
    ACTION_ALARM,
    ACTION_WRITE_TAG,
    ACTION_EXEC_CMD,
    ACTION_SEND_NOTIFY
} action_type_t;

/* 动作 */
typedef struct {
    action_type_t type;
    char params[256];
    data_value_t value;
} action_t;

/* 规则 */
typedef struct {
    char name[64];
    bool enabled;
    condition_t conditions[MAX_CONDITIONS];
    uint8_t condition_count;
    bool logic_and;             /* true=AND, false=OR */
    action_t actions[MAX_ACTIONS];
    uint8_t action_count;
    bool triggered;
    uint64_t trigger_count;
    uint64_t last_trigger;
    uint32_t cooldown_ms;       /* 冷却时间 */
} rule_t;

/* 规则引擎 */
typedef struct {
    tag_t *tags;
    uint32_t tag_count;
    rule_t rules[MAX_RULES];
    uint32_t rule_count;
    void (*alarm_callback)(const char *, const char *);
} rule_engine_t;

/* 全局引擎 */
static rule_engine_t g_engine = {0};

/**
 * @brief 获取当前时间 (毫秒)
 */
static uint64_t get_time_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

/**
 * @brief 查找测点
 */
static tag_t *find_tag(const char *name)
{
    for (uint32_t i = 0; i < g_engine.tag_count; i++) {
        if (strcmp(g_engine.tags[i].name, name) == 0) {
            return &g_engine.tags[i];
        }
    }
    return NULL;
}

/**
 * @brief 设置测点值
 */
void set_tag_value(const char *name, const data_value_t *value)
{
    tag_t *tag = find_tag(name);
    if (!tag) {
        return;
    }

    /* 检查值是否变化 */
    bool changed = false;
    if (tag->value.type != value->type) {
        changed = true;
    } else {
        switch (value->type) {
            case TYPE_BOOL:
                changed = (tag->value.data.b != value->data.b);
                break;
            case TYPE_INT:
                changed = (tag->value.data.i != value->data.i);
                break;
            case TYPE_FLOAT:
                changed = fabs(tag->value.data.f - value->data.f) > 0.0001;
                break;
            case TYPE_STRING:
                changed = strcmp(tag->value.data.s, value->data.s) != 0;
                break;
        }
    }

    tag->value = *value;
    tag->value.timestamp = get_time_ms();

    if (changed && tag->on_change) {
        tag->on_change(tag);
    }
}

/**
 * @brief 获取测点值
 */
bool get_tag_value(const char *name, data_value_t *value)
{
    tag_t *tag = find_tag(name);
    if (!tag) {
        return false;
    }
    *value = tag->value;
    return true;
}

/**
 * @brief 比较值
 */
static bool compare_values(const data_value_t *v1,
                            const data_value_t *v2,
                            operator_t op,
                            const data_value_t *v3)
{
    if (v1->type != v2->type) {
        return false;
    }

    double val1, val2, val3 = 0;

    /* 转换为浮点数比较 */
    switch (v1->type) {
        case TYPE_BOOL:
            val1 = v1->data.b ? 1.0 : 0.0;
            val2 = v2->data.b ? 1.0 : 0.0;
            if (v3) val3 = v3->data.b ? 1.0 : 0.0;
            break;
        case TYPE_INT:
            val1 = (double)v1->data.i;
            val2 = (double)v2->data.i;
            if (v3) val3 = (double)v3->data.i;
            break;
        case TYPE_FLOAT:
            val1 = v1->data.f;
            val2 = v2->data.f;
            if (v3) val3 = v3->data.f;
            break;
        case TYPE_STRING:
            int cmp = strcmp(v1->data.s, v2->data.s);
            switch (op) {
                case OP_EQ: return cmp == 0;
                case OP_NE: return cmp != 0;
                case OP_GT: return cmp > 0;
                case OP_GE: return cmp >= 0;
                case OP_LT: return cmp < 0;
                case OP_LE: return cmp <= 0;
                default: return false;
            }
    }

    switch (op) {
        case OP_EQ: return fabs(val1 - val2) < 0.0001;
        case OP_NE: return fabs(val1 - val2) >= 0.0001;
        case OP_GT: return val1 > val2;
        case OP_GE: return val1 >= val2;
        case OP_LT: return val1 < val2;
        case OP_LE: return val1 <= val2;
        case OP_IN_RANGE: return val1 >= val2 && val1 <= val3;
        case OP_OUT_RANGE: return val1 < val2 || val1 > val3;
        default: return false;
    }
}

/**
 * @brief 评估条件
 */
static bool evaluate_condition(condition_t *cond)
{
    data_value_t current;
    if (!get_tag_value(cond->tag_name, &current)) {
        return false;
    }

    bool result = compare_values(&current, &cond->threshold,
                                  cond->op,
                                  cond->op >= OP_IN_RANGE ? &cond->threshold2 : NULL);

    /* 处理持续时间 */
    if (cond->duration_ms > 0) {
        uint64_t now = get_time_ms();

        if (result) {
            if (!cond->last_result) {
                /* 首次触发 */
                cond->trigger_time = now;
                result = false;
            } else {
                /* 检查持续时间 */
                result = (now - cond->trigger_time) >= cond->duration_ms;
            }
        } else {
            cond->trigger_time = 0;
        }

        cond->last_result = result;
    }

    return result;
}

/**
 * @brief 执行动作
 */
static void execute_action(const action_t *action, const rule_t *rule)
{
    switch (action->type) {
        case ACTION_LOG:
            printf("[RULE:%s] LOG: %s\n", rule->name, action->params);
            break;

        case ACTION_ALARM:
            printf("[RULE:%s] ALARM: %s (Severity: %s)\n",
                   rule->name, action->params,
                   action->value.data.s);
            if (g_engine.alarm_callback) {
                g_engine.alarm_callback(rule->name, action->params);
            }
            break;

        case ACTION_WRITE_TAG:
            printf("[RULE:%s] WRITE: %s = ", rule->name, action->params);
            switch (action->value.type) {
                case TYPE_BOOL: printf("%s\n", action->value.data.b ? "true" : "false"); break;
                case TYPE_INT: printf("%ld\n", action->value.data.i); break;
                case TYPE_FLOAT: printf("%.2f\n", action->value.data.f); break;
                case TYPE_STRING: printf("%s\n", action->value.data.s); break;
            }
            set_tag_value(action->params, &action->value);
            break;

        case ACTION_EXEC_CMD:
            printf("[RULE:%s] EXEC: %s\n", rule->name, action->params);
            // system(action->params);
            break;

        case ACTION_SEND_NOTIFY:
            printf("[RULE:%s] NOTIFY: %s\n", rule->name, action->params);
            break;
    }
}

/**
 * @brief 评估规则
 */
static void evaluate_rule(rule_t *rule)
{
    if (!rule->enabled) {
        return;
    }

    /* 检查冷却时间 */
    if (rule->last_trigger > 0) {
        uint64_t elapsed = get_time_ms() - rule->last_trigger;
        if (elapsed < rule->cooldown_ms) {
            return;
        }
    }

    /* 评估条件 */
    bool conditions_met = rule->logic_and;

    for (uint8_t i = 0; i < rule->condition_count; i++) {
        bool result = evaluate_condition(&rule->conditions[i]);

        if (rule->logic_and) {
            conditions_met = conditions_met && result;
            if (!conditions_met) break;
        } else {
            conditions_met = conditions_met || result;
            if (conditions_met) break;
        }
    }

    /* 处理触发 */
    if (conditions_met && !rule->triggered) {
        rule->triggered = true;
        rule->trigger_count++;
        rule->last_trigger = get_time_ms();

        printf("\n[RULE:%s] TRIGGERED! (Count: %lu)\n",
               rule->name, rule->trigger_count);

        /* 执行动作 */
        for (uint8_t i = 0; i < rule->action_count; i++) {
            execute_action(&rule->actions[i], rule);
        }
    } else if (!conditions_met) {
        rule->triggered = false;
    }
}

/**
 * @brief 添加规则
 */
rule_t *add_rule(const char *name)
{
    if (g_engine.rule_count >= MAX_RULES) {
        return NULL;
    }

    rule_t *rule = &g_engine.rules[g_engine.rule_count++];
    memset(rule, 0, sizeof(rule_t));
    strncpy(rule->name, name, sizeof(rule->name) - 1);
    rule->enabled = true;
    rule->logic_and = true;
    return rule;
}

/**
 * @brief 运行规则引擎
 */
void run_rule_engine(void)
{
    for (uint32_t i = 0; i < g_engine.rule_count; i++) {
        evaluate_rule(&g_engine.rules[i]);
    }
}

/**
 * @brief 示例: 告警回调
 */
static void on_alarm(const char *rule_name, const char *message)
{
    printf("[ALARM HANDLER] Rule '%s': %s\n", rule_name, message);
    /* 这里可以发送邮件、短信等 */
}

/**
 * @brief 主函数
 */
int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║          Edge Rule Engine Demo                           ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* 初始化引擎 */
    g_engine.alarm_callback = on_alarm;

    /* 创建测点 */
    tag_t tags[] = {
        {.name = "Temperature", .value = {.type = TYPE_FLOAT, .data.f = 25.0}},
        {.name = "Pressure", .value = {.type = TYPE_FLOAT, .data.f = 1.0}},
        {.name = "MotorSpeed", .value = {.type = TYPE_INT, .data.i = 0}},
        {.name = "AlarmActive", .value = {.type = TYPE_BOOL, .data.b = false}},
    };
    g_engine.tags = tags;
    g_engine.tag_count = sizeof(tags) / sizeof(tags[0]);

    /* 规则1: 温度超高报警 */
    rule_t *rule1 = add_rule("HighTemperatureAlarm");
    if (rule1) {
        rule1->cooldown_ms = 5000;  /* 5秒冷却 */

        /* 条件: Temperature > 80 */
        condition_t *c1 = &rule1->conditions[rule1->condition_count++];
        strncpy(c1->tag_name, "Temperature", sizeof(c1->tag_name) - 1);
        c1->op = OP_GT;
        c1->threshold.type = TYPE_FLOAT;
        c1->threshold.data.f = 80.0;
        c1->duration_ms = 2000;  /* 持续2秒 */

        /* 动作1: 记录日志 */
        action_t *a1 = &rule1->actions[rule1->action_count++];
        a1->type = ACTION_LOG;
        strncpy(a1->params, "Temperature exceeds threshold", sizeof(a1->params) - 1);

        /* 动作2: 触发告警 */
        action_t *a2 = &rule1->actions[rule1->action_count++];
        a2->type = ACTION_ALARM;
        strncpy(a2->params, "High temperature detected", sizeof(a2->params) - 1);
        a2->value.type = TYPE_STRING;
        strncpy(a2->value.data.s, "CRITICAL", sizeof(a2->value.data.s) - 1);

        /* 动作3: 设置告警标志 */
        action_t *a3 = &rule1->actions[rule1->action_count++];
        a3->type = ACTION_WRITE_TAG;
        strncpy(a3->params, "AlarmActive", sizeof(a3->params) - 1);
        a3->value.type = TYPE_BOOL;
        a3->value.data.b = true;
    }

    /* 规则2: 压力范围检查 */
    rule_t *rule2 = add_rule("PressureRangeCheck");
    if (rule2) {
        /* 条件: Pressure < 0.5 OR Pressure > 2.0 */
        condition_t *c1 = &rule2->conditions[rule2->condition_count++];
        strncpy(c1->tag_name, "Pressure", sizeof(c1->tag_name) - 1);
        c1->op = OP_OUT_RANGE;
        c1->threshold.type = TYPE_FLOAT;
        c1->threshold.data.f = 0.5;
        c1->threshold2.type = TYPE_FLOAT;
        c1->threshold2.data.f = 2.0;

        /* 动作 */
        action_t *a1 = &rule2->actions[rule2->action_count++];
        a1->type = ACTION_LOG;
        strncpy(a1->params, "Pressure out of range", sizeof(a1->params) - 1);
    }

    /* 规则3: 电机启动 */
    rule_t *rule3 = add_rule("MotorControl");
    if (rule3) {
        /* 条件: Temperature < 60 AND Pressure < 1.5 */
        condition_t *c1 = &rule3->conditions[rule3->condition_count++];
        strncpy(c1->tag_name, "Temperature", sizeof(c1->tag_name) - 1);
        c1->op = OP_LT;
        c1->threshold.type = TYPE_FLOAT;
        c1->threshold.data.f = 60.0;

        condition_t *c2 = &rule3->conditions[rule3->condition_count++];
        strncpy(c2->tag_name, "Pressure", sizeof(c2->tag_name) - 1);
        c2->op = OP_LT;
        c2->threshold.type = TYPE_FLOAT;
        c2->threshold.data.f = 1.5;

        /* 动作: 启动电机 */
        action_t *a1 = &rule3->actions[rule3->action_count++];
        a1->type = ACTION_WRITE_TAG;
        strncpy(a1->params, "MotorSpeed", sizeof(a1->params) - 1);
        a1->value.type = TYPE_INT;
        a1->value.data.i = 1500;
    }

    printf("[INFO] %d rules created\n\n", g_engine.rule_count);

    /* 模拟数据变化并评估规则 */
    printf("=== Simulation Start ===\n\n");

    /* 场景1: 正常状态 */
    printf("[SCENE 1] Normal state\n");
    data_value_t v;
    v.type = TYPE_FLOAT; v.data.f = 25.0; set_tag_value("Temperature", &v);
    v.type = TYPE_FLOAT; v.data.f = 1.0; set_tag_value("Pressure", &v);
    run_rule_engine();
    printf("MotorSpeed: %ld\n\n", tags[2].value.data.i);

    /* 场景2: 温度升高 */
    printf("[SCENE 2] Temperature rising...\n");
    for (int i = 0; i < 10; i++) {
        v.type = TYPE_FLOAT;
        v.data.f = 25.0 + i * 6.0;  /* 25 -> 79 */
        set_tag_value("Temperature", &v);
        run_rule_engine();
        usleep(100000);
    }
    printf("\n");

    /* 场景3: 温度超过阈值 */
    printf("[SCENE 3] Temperature exceeds threshold\n");
    v.type = TYPE_FLOAT; v.data.f = 85.0; set_tag_value("Temperature", &v);
    run_rule_engine();
    usleep(2500000);  /* 等待2.5秒 */
    run_rule_engine();  /* 再次评估，触发持续条件 */
    printf("\n");

    /* 场景4: 压力异常 */
    printf("[SCENE 4] Pressure anomaly\n");
    v.type = TYPE_FLOAT; v.data.f = 0.3; set_tag_value("Pressure", &v);
    run_rule_engine();
    printf("\n");

    /* 场景5: 恢复正常 */
    printf("[SCENE 5] Return to normal\n");
    v.type = TYPE_FLOAT; v.data.f = 55.0; set_tag_value("Temperature", &v);
    v.type = TYPE_FLOAT; v.data.f = 1.2; set_tag_value("Pressure", &v);
    v.type = TYPE_BOOL; v.data.b = false; set_tag_value("AlarmActive", &v);
    run_rule_engine();
    printf("MotorSpeed: %ld\n\n", tags[2].value.data.i);

    printf("=== Simulation End ===\n");

    return 0;
}
