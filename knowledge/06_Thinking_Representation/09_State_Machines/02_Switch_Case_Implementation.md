# Switch-Case状态机实现模式

> **层级定位**: 06_Thinking_Representation > 09_State_Machines
> **难度级别**: L3 (中级)
> **前置知识**: [核心概念](01_Core_Concept_State_Machines.md), C语言基础
> **预估学习时间**: 2-3 小时

---

## 模块概述

Switch-Case是实现有限状态机(FSM)最经典、最直接的方式。
本模块深入讲解这种实现模式的设计原则、代码结构、常见陷阱和优化技巧，通过完整的工业级案例帮助你掌握这一核心技术。

---

## 学习目标

完成本模块后，你将能够：

1. **设计清晰的Switch-Case状态机结构** - 状态、事件、转换的规范组织
2. **处理复杂状态逻辑** - 入口/出口动作、守卫条件、转换动作
3. **避免常见陷阱** - 状态爆炸、遗漏事件、隐式转换
4. **优化性能** - 减少分支预测失败、优化代码布局

---

## 一、基础结构

### 1.1 标准模板

```c
/**
 * @file switch_case_fsm_template.c
 * @brief Switch-Case状态机标准模板
 * @compile: gcc -o fsm_template switch_case_fsm_template.c
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

/* ========== 类型定义 ========== */

typedef enum {
    STATE_IDLE,
    STATE_RUNNING,
    STATE_PAUSED,
    STATE_ERROR,
    STATE_COUNT  /* 状态总数，用于边界检查 */
} SystemState;

typedef enum {
    EVT_START,
    EVT_STOP,
    EVT_PAUSE,
    EVT_RESUME,
    EVT_ERROR,
    EVT_RESET,
    EVT_COUNT  /* 事件总数 */
} SystemEvent;

typedef struct {
    SystemState state;
    uint32_t entry_count;    /* 进入状态次数统计 */
    uint32_t error_code;     /* 错误代码 */
} FSMContext;

/* ========== 辅助函数 ========== */

const char* state_name(SystemState s) {
    static const char* names[] = {
        [STATE_IDLE]    = "IDLE",
        [STATE_RUNNING] = "RUNNING",
        [STATE_PAUSED]  = "PAUSED",
        [STATE_ERROR]   = "ERROR"
    };
    return (s < STATE_COUNT) ? names[s] : "UNKNOWN";
}

const char* event_name(SystemEvent e) {
    static const char* names[] = {
        [EVT_START]  = "START",
        [EVT_STOP]   = "STOP",
        [EVT_PAUSE]  = "PAUSE",
        [EVT_RESUME] = "RESUME",
        [EVT_ERROR]  = "ERROR",
        [EVT_RESET]  = "RESET"
    };
    return (e < EVT_COUNT) ? names[e] : "UNKNOWN";
}

/* ========== 状态机核心 ========== */

void fsm_init(FSMContext* ctx) {
    ctx->state = STATE_IDLE;
    ctx->entry_count = 0;
    ctx->error_code = 0;
    printf("[FSM] 初始化完成，初始状态: %s\n", state_name(ctx->state));
}

/* 入口动作 */
static void on_enter_state(FSMContext* ctx, SystemState new_state) {
    ctx->entry_count++;
    printf("[ENTRY] 进入状态: %s (第%d次)\n",
           state_name(new_state), ctx->entry_count);

    switch (new_state) {
        case STATE_RUNNING:
            printf("  → 启动系统资源\n");
            break;
        case STATE_PAUSED:
            printf("  → 保存当前进度\n");
            break;
        case STATE_ERROR:
            printf("  → 记录错误代码: 0x%08X\n", ctx->error_code);
            break;
        default:
            break;
    }
}

/* 出口动作 */
static void on_exit_state(FSMContext* ctx, SystemState old_state) {
    printf("[EXIT] 离开状态: %s\n", state_name(old_state));

    switch (old_state) {
        case STATE_RUNNING:
            printf("  → 停止系统资源\n");
            break;
        default:
            break;
    }
}

/* 主状态机处理函数 */
bool fsm_handle_event(FSMContext* ctx, SystemEvent event) {
    SystemState prev_state = ctx->state;
    SystemState next_state = ctx->state;  /* 默认保持当前状态 */
    bool transition_occurred = false;

    printf("[EVENT] 当前=%s, 事件=%s\n",
           state_name(ctx->state), event_name(event));

    switch (ctx->state) {
        /* ==================== IDLE状态 ==================== */
        case STATE_IDLE:
            switch (event) {
                case EVT_START:
                    next_state = STATE_RUNNING;
                    transition_occurred = true;
                    break;

                case EVT_RESET:
                    /* 已在IDLE，重置只是清理 */
                    printf("  → 清理上下文\n");
                    ctx->entry_count = 0;
                    break;

                default:
                    printf("  ⚠ 忽略无效事件 %s 在 IDLE 状态\n",
                           event_name(event));
                    break;
            }
            break;

        /* ==================== RUNNING状态 ==================== */
        case STATE_RUNNING:
            switch (event) {
                case EVT_STOP:
                    next_state = STATE_IDLE;
                    transition_occurred = true;
                    break;

                case EVT_PAUSE:
                    next_state = STATE_PAUSED;
                    transition_occurred = true;
                    break;

                case EVT_ERROR:
                    ctx->error_code = 1;  /* 示例错误码 */
                    next_state = STATE_ERROR;
                    transition_occurred = true;
                    break;

                default:
                    printf("  ⚠ 忽略无效事件 %s 在 RUNNING 状态\n",
                           event_name(event));
                    break;
            }
            break;

        /* ==================== PAUSED状态 ==================== */
        case STATE_PAUSED:
            switch (event) {
                case EVT_RESUME:
                    next_state = STATE_RUNNING;
                    transition_occurred = true;
                    break;

                case EVT_STOP:
                    next_state = STATE_IDLE;
                    transition_occurred = true;
                    break;

                default:
                    printf("  ⚠ 忽略无效事件 %s 在 PAUSED 状态\n",
                           event_name(event));
                    break;
            }
            break;

        /* ==================== ERROR状态 ==================== */
        case STATE_ERROR:
            switch (event) {
                case EVT_RESET:
                    ctx->error_code = 0;
                    next_state = STATE_IDLE;
                    transition_occurred = true;
                    break;

                default:
                    printf("  ⚠ 错误状态下只允许RESET，忽略 %s\n",
                           event_name(event));
                    break;
            }
            break;

        /* ==================== 默认/无效状态 ==================== */
        default:
            printf("  ❌ 致命错误: 无效状态 %d\n", ctx->state);
            ctx->error_code = 0xDEAD;
            next_state = STATE_ERROR;
            transition_occurred = true;
            break;
    }

    /* 执行状态转换 */
    if (transition_occurred && next_state != prev_state) {
        on_exit_state(ctx, prev_state);
        ctx->state = next_state;
        on_enter_state(ctx, next_state);
    }

    return transition_occurred;
}

/* ========== 测试场景 ========== */

int main(void) {
    FSMContext ctx;

    printf("\n╔═══════════════════════════════════════════════════════╗\n");
    printf("║         Switch-Case状态机 - 完整测试场景               ║\n");
    printf("╚═══════════════════════════════════════════════════════╝\n\n");

    /* 场景1: 正常启动-停止流程 */
    printf("【场景1】正常启动-停止流程\n");
    printf("────────────────────────────────────────\n");
    fsm_init(&ctx);
    fsm_handle_event(&ctx, EVT_START);
    fsm_handle_event(&ctx, EVT_STOP);
    printf("\n");

    /* 场景2: 启动-暂停-恢复-停止 */
    printf("【场景2】启动-暂停-恢复-停止\n");
    printf("────────────────────────────────────────\n");
    fsm_init(&ctx);
    fsm_handle_event(&ctx, EVT_START);
    fsm_handle_event(&ctx, EVT_PAUSE);
    fsm_handle_event(&ctx, EVT_RESUME);
    fsm_handle_event(&ctx, EVT_STOP);
    printf("\n");

    /* 场景3: 无效事件测试 */
    printf("【场景3】无效事件测试\n");
    printf("────────────────────────────────────────\n");
    fsm_init(&ctx);
    fsm_handle_event(&ctx, EVT_PAUSE);   /* IDLE时PAUSE无效 */
    fsm_handle_event(&ctx, EVT_START);
    fsm_handle_event(&ctx, EVT_START);   /* RUNNING时START无效 */
    printf("\n");

    /* 场景4: 错误处理 */
    printf("【场景4】错误处理与恢复\n");
    printf("────────────────────────────────────────\n");
    fsm_init(&ctx);
    fsm_handle_event(&ctx, EVT_START);
    fsm_handle_event(&ctx, EVT_ERROR);
    fsm_handle_event(&ctx, EVT_START);   /* ERROR时START无效 */
    fsm_handle_event(&ctx, EVT_RESET);
    printf("\n");

    printf("【统计】总状态进入次数: %d\n", ctx.entry_count);

    return 0;
}
```

### 1.2 代码结构分析

```
┌─────────────────────────────────────────────────────────────┐
│                    Switch-Case FSM结构                       │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  ┌──────────────┐     ┌──────────────┐     ┌──────────────┐ │
│  │   类型定义    │────▶│   辅助函数    │────▶│   核心FSM    │ │
│  │  State/Event │     │ 名字/日志等   │     │ handle_event │ │
│  └──────────────┘     └──────────────┘     └──────┬───────┘ │
│                                                    │         │
│                           ┌────────────────────────┘         │
│                           ▼                                  │
│              ┌────────────────────────┐                     │
│              │    switch(当前状态)     │                     │
│              └───────────┬────────────┘                     │
│                          │                                  │
│          ┌───────────────┼───────────────┐                  │
│          ▼               ▼               ▼                  │
│     ┌─────────┐    ┌─────────┐    ┌─────────┐              │
│     │ STATE_1 │    │ STATE_2 │    │ STATE_N │              │
│     └────┬────┘    └────┬────┘    └────┬────┘              │
│          │              │              │                    │
│          ▼              ▼              ▼                    │
│     ┌─────────┐    ┌─────────┐    ┌─────────┐              │
│     │switch   │    │switch   │    │switch   │              │
│     │(event)  │    │(event)  │    │(event)  │              │
│     └─────────┘    └─────────┘    └─────────┘              │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

---

## 二、进阶技术

### 2.1 带守卫条件的转换

```c
/**
 * @brief 带守卫条件的状态转换
 *
 * 在实际系统中，状态转换往往需要满足特定条件
 */

typedef struct {
    SystemState state;
    uint32_t runtime_ms;     /* 运行时间 */
    uint32_t temperature;    /* 温度传感器读数 */
    bool emergency_stop;     /* 紧急停止信号 */
} GuardedContext;

bool fsm_handle_event_guarded(GuardedContext* ctx, SystemEvent event) {
    switch (ctx->state) {
        case STATE_RUNNING:
            switch (event) {
                case EVT_STOP:
                    /* 守卫条件: 只有在运行超过1秒后才允许停止 */
                    if (ctx->runtime_ms < 1000) {
                        printf("  ⚠ 运行时间不足，拒绝STOP请求\n");
                        return false;
                    }
                    ctx->state = STATE_IDLE;
                    return true;

                case EVT_ERROR:
                    /* 守卫条件: 高温自动触发错误 */
                    if (ctx->temperature > 80) {
                        printf("  🔥 温度过高(%d°C)，触发保护!\n",
                               ctx->temperature);
                        ctx->state = STATE_ERROR;
                        return true;
                    }
                    /* 或者紧急停止信号 */
                    if (ctx->emergency_stop) {
                        printf("  🚨 紧急停止信号!\n");
                        ctx->state = STATE_ERROR;
                        return true;
                    }
                    break;

                default:
                    break;
            }
            break;

        default:
            break;
    }
    return false;
}
```

### 2.2 动作与转换分离

```c
/**
 * @brief 清晰的分离: 转换决策 vs 动作执行
 */

typedef enum {
    ACTION_NONE,
    ACTION_START_MOTOR,
    ACTION_STOP_MOTOR,
    ACTION_LOG_EVENT,
    ACTION_ALERT
} Action;

typedef struct {
    SystemState next_state;
    Action action;
    const char* reason;
} TransitionResult;

/* 纯决策函数: 无副作用，便于测试 */
TransitionResult fsm_decide_transition(
    SystemState current,
    SystemEvent event,
    const FSMContext* ctx
) {
    TransitionResult result = {
        .next_state = current,  /* 默认保持 */
        .action = ACTION_NONE,
        .reason = "no_transition"
    };

    switch (current) {
        case STATE_IDLE:
            if (event == EVT_START) {
                result.next_state = STATE_RUNNING;
                result.action = ACTION_START_MOTOR;
                result.reason = "start_requested";
            }
            break;

        case STATE_RUNNING:
            if (event == EVT_STOP) {
                result.next_state = STATE_IDLE;
                result.action = ACTION_STOP_MOTOR;
                result.reason = "stop_requested";
            } else if (event == EVT_ERROR) {
                result.next_state = STATE_ERROR;
                result.action = ACTION_ALERT;
                result.reason = "error_occurred";
            }
            break;

        default:
            break;
    }

    return result;
}

/* 动作执行函数 */
void execute_action(Action action, FSMContext* ctx) {
    switch (action) {
        case ACTION_START_MOTOR:
            printf("  [ACTION] 启动电机\n");
            /* 硬件操作... */
            break;

        case ACTION_STOP_MOTOR:
            printf("  [ACTION] 停止电机\n");
            /* 硬件操作... */
            break;

        case ACTION_ALERT:
            printf("  [ACTION] 发送告警\n");
            /* 通知系统... */
            break;

        default:
            break;
    }
}

/* 组合使用 */
void fsm_step(FSMContext* ctx, SystemEvent event) {
    TransitionResult trans = fsm_decide_transition(
        ctx->state, event, ctx);

    if (trans.next_state != ctx->state) {
        printf("[TRANS] %s → %s (%s)\n",
               state_name(ctx->state),
               state_name(trans.next_state),
               trans.reason);

        execute_action(trans.action, ctx);
        ctx->state = trans.next_state;
    }
}
```

---

## 三、常见陷阱与解决方案

### 陷阱对照表

| 陷阱 | 症状 | 解决方案 | 代码示例 |
|:-----|:-----|:---------|:---------|
| **忘记break** | 意外贯穿到下一个case | 使用static analyzer; 代码审查 | 见3.1 |
| **遗漏默认case** | 未处理意外状态 | 始终添加default处理 | 见3.2 |
| **状态爆炸** | >20个状态的巨大switch | 考虑层次状态机或查表法 | 见3.3 |
| **隐式转换** | 不明确的状态跳转 | 显式列出所有转换 | 见3.4 |
| **在状态内阻塞** | 长时间操作卡住状态机 | 使用异步或分解为子状态 | 见3.5 |

### 3.1 忘记break陷阱

```c
/* ❌ 错误: 忘记break导致贯穿 */
case STATE_A:
    if (event == EVT_1) {
        state = STATE_B;
    }
    /* 错误: 这里没有break! */
case STATE_B:  /* STATE_A的处理会意外执行到这里 */
    ...

/* ✅ 正确: 每个case都以break结束 */
case STATE_A:
    if (event == EVT_1) {
        state = STATE_B;
    }
    break;  /* 明确结束 */
```

### 3.2 防御性编程

```c
bool fsm_handle_event_defensive(FSMContext* ctx, SystemEvent event) {
    /* 前置条件检查 */
    if (!ctx) {
        fprintf(stderr, "[ERROR] 上下文为空\n");
        return false;
    }

    if (ctx->state >= STATE_COUNT) {
        fprintf(stderr, "[ERROR] 无效状态: %d\n", ctx->state);
        ctx->state = STATE_ERROR;  /* 恢复到安全状态 */
        return false;
    }

    if (event >= EVT_COUNT) {
        fprintf(stderr, "[WARN] 未知事件: %d，忽略\n", event);
        return false;
    }

    SystemState prev_state = ctx->state;

    switch (ctx->state) {
        /* ... 正常处理 ... */

        default:
            /* 捕获所有未明确处理的状态 */
            fprintf(stderr, "[ERROR] 未处理状态: %d\n", ctx->state);
            ctx->state = STATE_ERROR;
            break;
    }

    /* 后置条件检查 */
    if (ctx->state >= STATE_COUNT) {
        fprintf(stderr, "[ERROR] 转换后状态无效\n");
        ctx->state = STATE_ERROR;
    }

    return ctx->state != prev_state;
}
```

### 3.3 状态爆炸的处理

当状态数超过15-20个时，Switch-Case变得难以维护：

```c
/* 状态爆炸前的预警信号 */
#define STATE_COUNT 25  /* 警告: 状态数过多 */

/* 解决方案1: 状态分组 */
typedef enum {
    /* 主状态 */
    MODE_IDLE,
    MODE_ACTIVE,
    MODE_ERROR,
    /* 子状态编码在高位 */
    SUBSTATE_MASK = 0xFF00
} MainMode;

/* 解决方案2: 过渡到查表法或函数指针 */
/* 详见03_Table_Driven_Implementation.md */
/* 详见04_Function_Pointer_Implementation.md */
```

---

## 四、性能优化

### 4.1 分支预测优化

```c
/* GCC/Clang扩展: 提供分支预测提示 */
#if defined(__GNUC__)
    #define LIKELY(x)   __builtin_expect(!!(x), 1)
    #define UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
    #define LIKELY(x)   (x)
    #define UNLIKELY(x) (x)
#endif

/* 将最常见的情况放在前面 */
switch (state) {
    case STATE_RUNNING:  /* 假设90%时间在这里 */
        if (LIKELY(event == EVT_DATA)) {
            process_data();
        } else {
            handle_rare_events(event);
        }
        break;

    case STATE_IDLE:     /* 5%时间 */
        ...
        break;

    case STATE_ERROR:    /* 1%时间 */
        ...
        break;
}
```

### 4.2 性能对比

| 实现方式 | 时间复杂度 | 空间复杂度 | 分支预测 | 适用场景 |
|:---------|:----------:|:----------:|:--------:|:---------|
| Switch-Case | O(1) | 低 | 好(编译器优化) | <15状态 |
| 查表法 | O(1) | 中 | 优秀 | >10状态 |
| 函数指针 | O(1) | 高 | 优秀 | 复杂逻辑 |

---

## 五、思维表征: 决策流程

### 何时使用Switch-Case实现?

```
┌─────────────────────────────────────────────────────────────┐
│                    Switch-Case适用性决策                      │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  状态数 < 10? ──否──▶ 考虑查表法 ──▶ 状态数 > 20?           │
│      │                                     │                │
│     是                                    是                │
│      │                                     │                │
│      ▼                                     ▼                │
│  ┌─────────┐                         ┌─────────────┐        │
│  │使用Switch│                         │考虑层次状态机│        │
│  │  -Case   │                         │   或函数指针  │        │
│  └────┬────┘                         └─────────────┘        │
│       │                                                      │
│       ▼                                                      │
│  需要复杂的入口/出口动作?                                     │
│       │                                                      │
│   是──┴──否                                                  │
│   │        │                                                 │
│   ▼        ▼                                                 │
│  可以    推荐                                                │
│  使用    Switch-Case                                         │
│  宏简化                                                          │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

---

## 六、完整工业案例: 通信协议解析器

```c
/**
 * @file protocol_parser_fsm.c
 * @brief 基于Switch-Case的通信协议解析器
 * @description 处理帧头-长度-数据-校验的协议格式
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    PARSER_IDLE,
    PARSER_HEADER,
    PARSER_LENGTH,
    PARSER_DATA,
    PARSER_CHECKSUM,
    PARSER_COMPLETE,
    PARSER_ERROR
} ParserState;

typedef struct {
    ParserState state;
    uint8_t buffer[256];
    uint8_t length;
    uint8_t received;
    uint8_t checksum;
} ParserContext;

#define FRAME_HEADER 0xAA

void parser_init(ParserContext* ctx) {
    ctx->state = PARSER_IDLE;
    ctx->length = 0;
    ctx->received = 0;
    ctx->checksum = 0;
}

bool parser_feed_byte(ParserContext* ctx, uint8_t byte) {
    switch (ctx->state) {
        case PARSER_IDLE:
            if (byte == FRAME_HEADER) {
                ctx->state = PARSER_HEADER;
                ctx->checksum = byte;
            }
            break;

        case PARSER_HEADER:
            ctx->length = byte;
            ctx->checksum += byte;
            if (ctx->length > 0 && ctx->length <= 256) {
                ctx->state = PARSER_LENGTH;
                ctx->received = 0;
            } else {
                ctx->state = PARSER_ERROR;
            }
            break;

        case PARSER_LENGTH:
            ctx->buffer[ctx->received++] = byte;
            ctx->checksum += byte;
            if (ctx->received >= ctx->length) {
                ctx->state = PARSER_DATA;
            }
            break;

        case PARSER_DATA:
            /* 校验和验证 */
            if (byte == ctx->checksum) {
                ctx->state = PARSER_COMPLETE;
                return true;  /* 帧完整 */
            } else {
                ctx->state = PARSER_ERROR;
            }
            break;

        case PARSER_COMPLETE:
        case PARSER_ERROR:
            /* 重置并检查新帧头 */
            parser_init(ctx);
            if (byte == FRAME_HEADER) {
                ctx->state = PARSER_HEADER;
                ctx->checksum = byte;
            }
            break;

        default:
            ctx->state = PARSER_ERROR;
            break;
    }

    return false;
}

int main(void) {
    ParserContext parser;
    parser_init(&parser);

    /* 测试数据: AA 03 11 22 33 69 (AA=头, 03=长度, 11 22 33=数据, 69=校验和) */
    uint8_t test_frame[] = {0xAA, 0x03, 0x11, 0x22, 0x33, 0x69};

    printf("协议解析器测试:\n");
    for (size_t i = 0; i < sizeof(test_frame); i++) {
        printf("  输入: 0x%02X -> ", test_frame[i]);
        if (parser_feed_byte(&parser, test_frame[i])) {
            printf("✓ 帧接收完成! 长度=%d\n", parser.length);
        } else {
            printf("状态=%d\n", parser.state);
        }
    }

    return 0;
}
```

---

## 七、知识检验

### 自测题

1. **为什么要在每个case后加break? 如果故意要贯穿(fall-through)怎么办?**

2. **入口动作和出口动作在什么时候执行? 为什么要有这些动作?**

3. **如何测试一个Switch-Case状态机的所有路径?**

4. **当状态数超过20个时，Switch-Case的主要问题是什么?**

### 代码练习

**练习: 交通灯控制器**

实现一个十字路口交通灯的状态机：

```c
typedef enum {
    NS_GREEN_EW_RED,    /* 南北绿，东西红 */
    NS_YELLOW_EW_RED,   /* 南北黄，东西红 */
    NS_RED_EW_GREEN,    /* 南北红，东西绿 */
    NS_RED_EW_YELLOW    /* 南北红，东西黄 */
} TrafficState;

/* 要求:
 * 1. 绿灯持续30秒，黄灯5秒
 * 2. 支持紧急车辆优先(切换到指定方向绿灯)
 * 3. 使用Switch-Case实现
 * 4. 包含入口动作(灯切换时输出日志)
 */
```

---

## 八、延伸阅读

### 相关模块

- [查表法实现](03_Table_Driven_Implementation.md) - 状态数较多时的替代方案
- [函数指针实现](04_Function_Pointer_Implementation.md) - 复杂逻辑的首选
- [层次状态机](05_Hierarchical_State_Machines.md) - 状态组织的高级模式

### 参考资源

1. **"Practical UML Statecharts in C/C++"** - Miro Samek
2. **QP-nano Framework** - 嵌入式状态机框架源码

---

**最后更新**: 2026-03-24
**维护者**: C_Lang Knowledge Base Team
**质量等级**: L3 (深化)
