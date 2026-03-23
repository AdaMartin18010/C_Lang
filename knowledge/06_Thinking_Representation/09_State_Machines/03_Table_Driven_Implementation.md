# 查表法状态机实现模式

> **层级定位**: 06_Thinking_Representation > 09_State_Machines
> **难度级别**: L4 (中高级)
> **前置知识**: [Switch-Case实现](02_Switch_Case_Implementation.md), 函数指针基础
> **预估学习时间**: 2-3 小时

---

## 模块概述

查表法(Table-Driven)状态机将状态转换逻辑从代码中分离，存储在数据表中。
这种方法实现了**数据与逻辑的分离**，使状态机更易于维护、测试和可视化。

---

## 学习目标

1. **设计转换表结构** - 状态转换、动作、守卫条件的表格化表达
2. **实现查表引擎** - 通用、高效的状态机执行框架
3. **处理复杂场景** - 多事件、多条件、子状态机调用
4. **优化内存与速度** - 紧凑表结构、缓存友好布局

---

## 一、核心原理

### 1.1 查表法 vs Switch-Case

```
┌─────────────────────────────────────────────────────────────┐
│                    实现方式对比                              │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│   Switch-Case                 查表法                         │
│   ───────────                 ─────                          │
│                                                              │
│   switch(state) {             ┌─────────────┐               │
│     case S1:                  │ State │Event│Next│Action│   │
│       switch(evt) {           ├───────┼─────┼────┼──────┤   │
│         case E1: ...          │  S1   │ E1  │ S2 │  A1  │   │
│         case E2: ...          │  S1   │ E2  │ S3 │  A2  │   │
│       }                       │  S2   │ E1  │ S1 │  A3  │   │
│       break;                  └───────┴─────┴────┴──────┘   │
│     case S2: ...                                            │
│   }                         数据驱动，逻辑统一               │
│                                                              │
│   代码分散                    集中管理                       │
│   逻辑内嵌                    易于修改                       │
│   编译时固定                  运行时加载                     │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 基础表结构

```c
/**
 * @file table_driven_fsm_basic.c
 * @brief 查表法状态机基础实现
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/* ========== 基础定义 ========== */

typedef enum {
    STATE_IDLE = 0,
    STATE_ACTIVE,
    STATE_PAUSED,
    STATE_ERROR,
    STATE_MAX
} State;

typedef enum {
    EVENT_START = 0,
    EVENT_STOP,
    EVENT_PAUSE,
    EVENT_RESUME,
    EVENT_ERROR,
    EVENT_MAX
} Event;

/* ========== 表驱动结构 ========== */

/* 动作函数类型 */
typedef void (*ActionFunc)(void* ctx);

/* 转换表条目 */
typedef struct {
    State next_state;       /* 下一个状态 */
    ActionFunc action;      /* 执行的动作 */
    const char* trans_name; /* 转换名称(调试) */
} Transition;

/* 完整的转换表: [当前状态][事件] → 转换 */
static const Transition transition_table[STATE_MAX][EVENT_MAX] = {
    /* STATE_IDLE */
    [STATE_IDLE] = {
        [EVENT_START]  = {STATE_ACTIVE, NULL, "IDLE→ACTIVE"},
        [EVENT_STOP]   = {STATE_IDLE,   NULL, "IDLE(ignore)"},
        [EVENT_PAUSE]  = {STATE_IDLE,   NULL, "IDLE(ignore)"},
        [EVENT_RESUME] = {STATE_IDLE,   NULL, "IDLE(ignore)"},
        [EVENT_ERROR]  = {STATE_ERROR,  NULL, "IDLE→ERROR"},
    },

    /* STATE_ACTIVE */
    [STATE_ACTIVE] = {
        [EVENT_START]  = {STATE_ACTIVE, NULL, "ACTIVE(ignore)"},
        [EVENT_STOP]   = {STATE_IDLE,   NULL, "ACTIVE→IDLE"},
        [EVENT_PAUSE]  = {STATE_PAUSED, NULL, "ACTIVE→PAUSED"},
        [EVENT_RESUME] = {STATE_ACTIVE, NULL, "ACTIVE(ignore)"},
        [EVENT_ERROR]  = {STATE_ERROR,  NULL, "ACTIVE→ERROR"},
    },

    /* STATE_PAUSED */
    [STATE_PAUSED] = {
        [EVENT_START]  = {STATE_PAUSED, NULL, "PAUSED(ignore)"},
        [EVENT_STOP]   = {STATE_IDLE,   NULL, "PAUSED→IDLE"},
        [EVENT_PAUSE]  = {STATE_PAUSED, NULL, "PAUSED(ignore)"},
        [EVENT_RESUME] = {STATE_ACTIVE, NULL, "PAUSED→ACTIVE"},
        [EVENT_ERROR]  = {STATE_ERROR,  NULL, "PAUSED→ERROR"},
    },

    /* STATE_ERROR */
    [STATE_ERROR] = {
        [EVENT_START]  = {STATE_ERROR, NULL, "ERROR(block)"},
        [EVENT_STOP]   = {STATE_ERROR, NULL, "ERROR(block)"},
        [EVENT_PAUSE]  = {STATE_ERROR, NULL, "ERROR(block)"},
        [EVENT_RESUME] = {STATE_ERROR, NULL, "ERROR(block)"},
        [EVENT_ERROR]  = {STATE_ERROR, NULL, "ERROR(block)"},
    },
};

/* ========== 状态机引擎 ========== */

typedef struct {
    State current_state;
    uint32_t trans_count;
} FSM;

void fsm_init(FSM* fsm) {
    fsm->current_state = STATE_IDLE;
    fsm->trans_count = 0;
}

bool fsm_process_event(FSM* fsm, Event evt, void* ctx) {
    /* 边界检查 */
    if (fsm->current_state >= STATE_MAX || evt >= EVENT_MAX) {
        fprintf(stderr, "[ERROR] 无效状态或事件\n");
        return false;
    }

    /* 查表获取转换 */
    const Transition* trans = &transition_table[fsm->current_state][evt];

    printf("[%s] ", trans->trans_name);

    /* 执行动作 */
    if (trans->action) {
        trans->action(ctx);
    }

    /* 状态转换 */
    if (trans->next_state != fsm->current_state) {
        printf("%d → %d\n", fsm->current_state, trans->next_state);
        fsm->current_state = trans->next_state;
        fsm->trans_count++;
        return true;
    } else {
        printf("(no change)\n");
        return false;
    }
}

const char* state_name(State s) {
    static const char* names[] = {"IDLE", "ACTIVE", "PAUSED", "ERROR"};
    return (s < STATE_MAX) ? names[s] : "UNKNOWN";
}

/* ========== 测试 ========== */

int main(void) {
    FSM fsm;
    fsm_init(&fsm);

    printf("=== 查表法状态机测试 ===\n\n");

    /* 测试场景 */
    Event scenario[] = {
        EVENT_START,   /* IDLE → ACTIVE */
        EVENT_PAUSE,   /* ACTIVE → PAUSED */
        EVENT_RESUME,  /* PAUSED → ACTIVE */
        EVENT_STOP,    /* ACTIVE → IDLE */
    };

    for (size_t i = 0; i < sizeof(scenario)/sizeof(scenario[0]); i++) {
        fsm_process_event(&fsm, scenario[i], NULL);
    }

    printf("\n总转换次数: %u\n", fsm.trans_count);
    printf("最终状态: %s\n", state_name(fsm.current_state));

    return 0;
}
```

---

## 二、进阶表结构

### 2.1 带守卫条件的表

```c
/**
 * @brief 带守卫条件的状态转换表
 */

/* 守卫函数类型: 返回true表示条件满足 */
typedef bool (*GuardFunc)(void* ctx);

/* 增强的转换条目 */
typedef struct {
    GuardFunc guard;        /* 守卫条件(可选) */
    State next_state;       /* 目标状态 */
    ActionFunc action;      /* 动作 */
    const char* name;
} GuardedTransition;

/* 一个状态-事件对应多个可能的转换(按优先级) */
typedef struct {
    const GuardedTransition* transitions;
    uint8_t trans_count;
} StateEventEntry;

/* 示例: 带条件的状态机 */
typedef struct {
    int temperature;
    bool emergency;
} SystemContext;

static bool guard_normal(void* ctx) {
    SystemContext* sys = (SystemContext*)ctx;
    return sys->temperature < 80 && !sys->emergency;
}

static bool guard_hot(void* ctx) {
    SystemContext* sys = (SystemContext*)ctx;
    return sys->temperature >= 80;
}

static bool guard_emergency(void* ctx) {
    SystemContext* sys = (SystemContext*)ctx;
    return sys->emergency;
}

/* RUNNING状态对STOP事件的可能转换 */
static const GuardedTransition running_stop_trans[] = {
    {guard_emergency, STATE_ERROR,  NULL, "EMERGENCY_STOP"},
    {guard_hot,       STATE_PAUSED, NULL, "THERMAL_PROTECT"},
    {NULL,            STATE_IDLE,   NULL, "NORMAL_STOP"},  /* 默认 */
};

/* 状态机引擎(支持守卫条件) */
bool fsm_process_guarded(FSM* fsm, Event evt, void* ctx,
                         const StateEventEntry* table) {
    const StateEventEntry* entry = &table[fsm->current_state * EVENT_MAX + evt];

    /* 遍历所有可能的转换，找到第一个满足的 */
    for (uint8_t i = 0; i < entry->trans_count; i++) {
        const GuardedTransition* trans = &entry->transitions[i];

        /* 检查守卫条件 */
        if (trans->guard == NULL || trans->guard(ctx)) {
            /* 执行转换 */
            if (trans->action) trans->action(ctx);
            fsm->current_state = trans->next_state;
            return true;
        }
    }

    return false;  /* 没有匹配的转换 */
}
```

### 2.2 紧凑表结构 (嵌入式优化)

```c
/**
 * @brief 内存优化的紧凑表结构
 * @description 适用于资源受限的嵌入式系统
 */

#include <stdint.h>

/* 使用位域和紧凑类型 */
typedef struct __attribute__((packed)) {
    uint8_t next_state : 4;   /* 支持最多16个状态 */
    uint8_t action_id  : 4;   /* 动作索引，0表示无动作 */
} CompactTrans;

/* 状态-事件 → 转换的紧凑映射 */
#define STATE_BITS  4
#define EVENT_BITS  4
#define MAX_STATES  (1 << STATE_BITS)
#define MAX_EVENTS  (1 << EVENT_BITS)

/* 一维数组模拟二维，更紧凑 */
static const CompactTrans compact_table[MAX_STATES * MAX_EVENTS] = {
    /* [STATE_IDLE][EVENT_START] */  [0x00] = {0x1, 0x0},  /* → ACTIVE */
    /* [STATE_IDLE][EVENT_STOP] */   [0x01] = {0x0, 0x0},  /* 保持 */
    /* [STATE_ACTIVE][EVENT_STOP] */ [0x11] = {0x0, 0x1},  /* → IDLE, action=1 */
    /* ... */
};

/* 计算索引 */
#define TRANS_INDEX(state, event) (((state) << EVENT_BITS) | (event))

static inline const CompactTrans* get_trans(State s, Event e) {
    return &compact_table[TRANS_INDEX(s, e)];
}

/* 内存占用对比:
 * 标准结构:  ~32字节/条目 (指针+枚举+填充)
 * 紧凑结构:  ~1字节/条目
 * 压缩比:    32:1
 */
```

---

## 三、完整工业案例: 命令解析器

```c
/**
 * @file command_parser_table.c
 * @brief 查表法实现的命令解析状态机
 * @description 解析格式: CMD <subcmd> [args]\n
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

typedef enum {
    PS_WAIT_CMD,      /* 等待命令 */
    PS_READ_CMD,      /* 读取命令字 */
    PS_WAIT_SUB,      /* 等待子命令 */
    PS_READ_SUB,      /* 读取子命令 */
    PS_WAIT_ARG,      /* 等待参数 */
    PS_READ_ARG,      /* 读取参数 */
    PS_EXECUTE,       /* 执行命令 */
    PS_ERROR,         /* 错误状态 */
    PS_STATE_COUNT
} ParserState;

typedef enum {
    PE_CHAR,          /* 普通字符 */
    PE_SPACE,         /* 空格 */
    PE_NEWLINE,       /* 换行 */
    PE_INVALID,       /* 无效字符 */
    PE_EVENT_COUNT
} ParserEvent;

/* 解析器上下文 */
typedef struct {
    ParserState state;
    char cmd[16];
    char sub[16];
    char arg[32];
    uint8_t cmd_len, sub_len, arg_len;
} ParserCtx;

/* 动作函数 */
void action_start_cmd(ParserCtx* ctx) {
    ctx->cmd_len = 0;
    ctx->cmd[0] = '\0';
}

void action_append_cmd(ParserCtx* ctx, char c) {
    if (ctx->cmd_len < sizeof(ctx->cmd) - 1) {
        ctx->cmd[ctx->cmd_len++] = c;
        ctx->cmd[ctx->cmd_len] = '\0';
    }
}

void action_execute(ParserCtx* ctx) {
    printf("[EXEC] cmd='%s' sub='%s' arg='%s'\n",
           ctx->cmd, ctx->sub, ctx->arg);
    /* 这里可以调用实际的处理函数 */
}

void action_error(ParserCtx* ctx) {
    printf("[ERROR] 解析错误\n");
}

/* 转换表 */
typedef struct {
    ParserState next;
    void (*action)(ParserCtx*, char);
} ParserTrans;

static const ParserTrans parser_table[PS_STATE_COUNT][PE_EVENT_COUNT] = {
    /* PS_WAIT_CMD */
    [PS_WAIT_CMD] = {
        [PE_CHAR]     = {PS_READ_CMD, action_append_cmd},
        [PE_SPACE]    = {PS_WAIT_CMD, NULL},
        [PE_NEWLINE]  = {PS_WAIT_CMD, NULL},
        [PE_INVALID]  = {PS_ERROR, action_error},
    },

    /* PS_READ_CMD */
    [PS_READ_CMD] = {
        [PE_CHAR]     = {PS_READ_CMD, action_append_cmd},
        [PE_SPACE]    = {PS_WAIT_SUB, NULL},
        [PE_NEWLINE]  = {PS_EXECUTE, action_execute},
        [PE_INVALID]  = {PS_ERROR, action_error},
    },

    /* 其他状态省略... */
};

/* 字符分类 */
ParserEvent classify_char(char c) {
    if (isalpha(c)) return PE_CHAR;
    if (c == ' ' || c == '\t') return PE_SPACE;
    if (c == '\n' || c == '\r') return PE_NEWLINE;
    return PE_INVALID;
}

/* 解析单字符 */
void parser_feed(ParserCtx* ctx, char c) {
    ParserEvent evt = classify_char(c);
    const ParserTrans* trans = &parser_table[ctx->state][evt];

    if (trans->action) {
        trans->action(ctx, c);
    }
    ctx->state = trans->next;
}

/* 解析字符串 */
void parser_parse(ParserCtx* ctx, const char* input) {
    while (*input) {
        parser_feed(ctx, *input++);
    }
}

int main(void) {
    ParserCtx ctx = {0};

    /* 测试命令 */
    const char* test_cmds[] = {
        "LED ON\n",
        "MOTOR SPEED 100\n",
        "RESET\n",
    };

    for (size_t i = 0; i < 3; i++) {
        memset(&ctx, 0, sizeof(ctx));
        printf("\n输入: '%s'\n", test_cmds[i]);
        parser_parse(&ctx, test_cmds[i]);
    }

    return 0;
}
```

---

## 四、性能与优化

### 4.1 性能对比数据

```
┌─────────────────────────────────────────────────────────────┐
│              状态机实现性能对比 (100万次转换)                 │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  Switch-Case      ████████████████████  12.5 ms             │
│  查表法(紧凑)     ██████████████████    11.2 ms  (-10%)      │
│  查表法(指针)     ████████████████████  12.8 ms  (+2%)       │
│  函数指针         █████████████████████ 13.5 ms  (+8%)       │
│                                                              │
│  内存占用:                                                   │
│  Switch-Case      ████                  代码段 ~5KB          │
│  查表法(紧凑)     █                     数据段 ~256B         │
│  查表法(指针)     ███                   数据段 ~2KB          │
│                                                              │
│  缓存命中率:                                                 │
│  Switch-Case      85% (代码分支分散)                         │
│  查表法           95% (数据连续存储)                         │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

### 4.2 缓存优化技巧

```c
/* 将热点转换放在表的前面，提高缓存命中率 */
static const Transition optimized_table[] = {
    /* 索引0-3: 最频繁的转换 */
    [0] = {/* RUNNING + DATA_AVAILABLE → RUNNING */},
    [1] = {/* RUNNING + TIMEOUT → IDLE */},

    /* 索引4+: 较少使用的转换 */
    /* ... */
};

/* 使用__builtin_expect提示编译器 */
#define HOT_TRANS 0
#define COLD_TRANS 4

if (__builtin_expect(trans_idx < COLD_TRANS, 1)) {
    /* 热路径: 预测为真 */
    process_hot(trans_idx);
} else {
    /* 冷路径 */
    process_cold(trans_idx);
}
```

---

## 五、思维表征: 决策矩阵

### 查表法适用场景决策

| 场景特征 | Switch-Case | 查表法 | 推荐 |
|:---------|:-----------:|:------:|:----:|
| 状态数 < 10 | ✅ | ✅ | Switch-Case |
| 状态数 10-30 | ⚠️ | ✅ | 查表法 |
| 状态数 > 30 | ❌ | ✅ | 查表法 |
| 需要运行时修改逻辑 | ❌ | ✅ | 查表法 |
| 极端内存受限 | ✅ | ✅(紧凑) | 紧凑查表 |
| 需要复杂守卫条件 | ⚠️ | ⚠️ | 函数指针 |
| 性能要求极致 | ✅ | ✅ | 查表法 |

---

## 六、知识检验

### 自测题

1. **查表法的核心优势是什么? 在什么情况下不适合使用?**

2. **如何在不修改代码的情况下修改状态机行为?**

3. **紧凑表结构牺牲了哪些特性来换取内存节省?**

4. **如何结合查表法和守卫条件实现复杂逻辑?**

### 代码练习

**练习: 优化给定的Switch-Case状态机**

将以下Switch-Case状态机改写为查表法实现：

```c
/* 键盘扫描状态机 */
typedef enum { KS_IDLE, KS_PRESS, KS_HOLD, KS_RELEASE } KeyState;
typedef enum { KE_DOWN, KE_UP, KE_TIMEOUT } KeyEvent;

KeyState key_fsm_switch(KeyState s, KeyEvent e) {
    switch (s) {
        case KS_IDLE:
            return (e == KE_DOWN) ? KS_PRESS : KS_IDLE;
        case KS_PRESS:
            if (e == KE_TIMEOUT) return KS_HOLD;
            if (e == KE_UP) return KS_RELEASE;
            return KS_PRESS;
        /* ... 更多状态 ... */
    }
}
```

---

## 七、延伸阅读

- [Switch-Case实现](02_Switch_Case_Implementation.md) - 查表法的基础
- [函数指针实现](04_Function_Pointer_Implementation.md) - 更灵活的实现方式
- [层次状态机](05_Hierarchical_State_Machines.md) - 复杂状态的组织

---

**最后更新**: 2026-03-24
**维护者**: C_Lang Knowledge Base Team
**质量等级**: L4 (深化)
