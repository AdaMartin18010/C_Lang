# 状态机测试与调试

> **层级定位**: 06_Thinking_Representation > 09_State_Machines
> **难度级别**: L4 (中高级)
> **前置知识**: 状态机基础实现, C单元测试
> **预估学习时间**: 2-3 小时

---

## 模块概述

状态机的正确性至关重要——一个错误的状态转换可能导致系统崩溃。
本模块讲解如何系统地测试状态机，包括单元测试策略、调试技巧和可视化方法。

---

## 学习目标

1. **设计状态机测试用例** - 覆盖所有状态和转换
2. **实现自动化测试框架** - 可重复的状态机验证
3. **调试状态机问题** - 跟踪状态历史和事件序列
4. **可视化状态机执行** - 状态图、日志分析

---

## 一、测试策略

### 1.1 测试金字塔

```
┌─────────────────────────────────────────────────────────────┐
│                    状态机测试层次                            │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│     ┌─────────────┐                                         │
│     │  集成测试   │  与真实硬件/系统交互                    │
│     │  (10%)      │  • 端到端场景                          │
│     └──────┬──────┘  • 时序测试                            │
│            │                                                │
│     ┌──────┴──────┐                                         │
│     │  功能测试   │  状态机行为验证                         │
│     │  (30%)      │  • 所有状态转换                        │
│     └──────┬──────┘  • 边界条件                            │
│            │                                                │
│     ┌──────┴──────┐                                         │
│     │  单元测试   │  状态函数单独测试                       │
│     │  (60%)      │  • 每个状态的处理逻辑                  │
│     └─────────────┘  • 守卫条件                            │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 测试覆盖目标

| 覆盖类型 | 目标 | 验证方法 |
|:---------|:-----|:---------|
| **状态覆盖** | 每个状态至少访问一次 | 检查日志/断点 |
| **转换覆盖** | 每个有效转换至少执行一次 | 转换计数器 |
| **事件覆盖** | 每个事件在每个状态都被测试 | 事件矩阵 |
| **路径覆盖** | 关键状态序列被验证 | 场景测试 |
| **守卫覆盖** | 每个守卫的真/假分支都测试 | 条件测试 |

---

## 二、单元测试框架

### 2.1 最小测试框架

```c
/**
 * @file fsm_test_framework.h
 * @brief 轻量级状态机测试框架
 */

#ifndef FSM_TEST_FRAMEWORK_H
#define FSM_TEST_FRAMEWORK_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/* 测试结果统计 */
typedef struct {
    int passed;
    int failed;
    int total;
} TestStats;

static TestStats g_test_stats = {0, 0, 0};

/* 测试宏 */
#define TEST_ASSERT(cond, msg) do { \
    g_test_stats.total++; \
    if (cond) { \
        g_test_stats.passed++; \
        printf("  ✓ %s\n", msg); \
    } else { \
        g_test_stats.failed++; \
        printf("  ✗ %s (失败: %s)\n", msg, #cond); \
    } \
} while(0)

#define TEST_ASSERT_EQ(expected, actual, msg) \
    TEST_ASSERT((expected) == (actual), msg)

#define TEST_ASSERT_STR_EQ(expected, actual, msg) \
    TEST_ASSERT(strcmp(expected, actual) == 0, msg)

/* 测试运行 */
#define RUN_TEST(test_func) do { \
    printf("\n【测试】%s\n", #test_func); \
    test_func(); \
} while(0)

/* 报告生成 */
static inline void print_test_report(void) {
    printf("\n╔═══════════════════════════════════════╗\n");
    printf("║           测试报告                    ║\n");
    printf("╠═══════════════════════════════════════╣\n");
    printf("║  总计:  %3d                          ║\n", g_test_stats.total);
    printf("║  通过:  %3d  ✓                        ║\n", g_test_stats.passed);
    printf("║  失败:  %3d  ✗                        ║\n", g_test_stats.failed);
    printf("║  通过率: %5.1f%%                      ║\n",
           g_test_stats.total > 0 ?
           (100.0 * g_test_stats.passed / g_test_stats.total) : 0);
    printf("╚═══════════════════════════════════════╝\n");
}

#endif /* FSM_TEST_FRAMEWORK_H */
```

### 2.2 状态机专项测试

```c
/**
 * @file fsm_test_example.c
 * @brief 状态机测试示例
 */

#include "fsm_test_framework.h"

/* 被测试的状态机 */
typedef enum { ST_IDLE, ST_ACTIVE, ST_ERROR } State;
typedef enum { EV_START, EV_STOP, EV_ERROR, EV_RESET } Event;

typedef struct {
    State state;
    int entry_count;
    int error_code;
} TestFSM;

void fsm_init(TestFSM* fsm) {
    fsm->state = ST_IDLE;
    fsm->entry_count = 0;
    fsm->error_code = 0;
}

bool fsm_handle(TestFSM* fsm, Event ev) {
    switch (fsm->state) {
        case ST_IDLE:
            if (ev == EV_START) {
                fsm->state = ST_ACTIVE;
                fsm->entry_count++;
                return true;
            }
            break;
        case ST_ACTIVE:
            if (ev == EV_STOP) {
                fsm->state = ST_IDLE;
                return true;
            }
            if (ev == EV_ERROR) {
                fsm->state = ST_ERROR;
                fsm->error_code = 1;
                return true;
            }
            break;
        case ST_ERROR:
            if (ev == EV_RESET) {
                fsm->state = ST_IDLE;
                fsm->error_code = 0;
                return true;
            }
            break;
    }
    return false;
}

/* ========== 测试用例 ========== */

void test_initial_state(void) {
    TestFSM fsm;
    fsm_init(&fsm);

    TEST_ASSERT_EQ(ST_IDLE, fsm.state, "初始状态应为IDLE");
    TEST_ASSERT_EQ(0, fsm.entry_count, "初始计数应为0");
}

void test_idle_to_active(void) {
    TestFSM fsm;
    fsm_init(&fsm);

    bool result = fsm_handle(&fsm, EV_START);

    TEST_ASSERT(result, "START应触发转换");
    TEST_ASSERT_EQ(ST_ACTIVE, fsm.state, "应转换到ACTIVE");
    TEST_ASSERT_EQ(1, fsm.entry_count, "进入计数应增加");
}

void test_active_to_idle(void) {
    TestFSM fsm;
    fsm_init(&fsm);
    fsm_handle(&fsm, EV_START);

    bool result = fsm_handle(&fsm, EV_STOP);

    TEST_ASSERT(result, "STOP应触发转换");
    TEST_ASSERT_EQ(ST_IDLE, fsm.state, "应转换到IDLE");
}

void test_active_to_error(void) {
    TestFSM fsm;
    fsm_init(&fsm);
    fsm_handle(&fsm, EV_START);

    bool result = fsm_handle(&fsm, EV_ERROR);

    TEST_ASSERT(result, "ERROR应触发转换");
    TEST_ASSERT_EQ(ST_ERROR, fsm.state, "应转换到ERROR");
    TEST_ASSERT_EQ(1, fsm.error_code, "错误码应设置");
}

void test_error_to_idle(void) {
    TestFSM fsm;
    fsm_init(&fsm);
    fsm_handle(&fsm, EV_START);
    fsm_handle(&fsm, EV_ERROR);

    bool result = fsm_handle(&fsm, EV_RESET);

    TEST_ASSERT(result, "RESET应触发转换");
    TEST_ASSERT_EQ(ST_IDLE, fsm.state, "应转换到IDLE");
    TEST_ASSERT_EQ(0, fsm.error_code, "错误码应清除");
}

void test_invalid_transitions(void) {
    TestFSM fsm;
    fsm_init(&fsm);

    /* IDLE状态下不应响应STOP */
    bool result = fsm_handle(&fsm, EV_STOP);
    TEST_ASSERT(!result, "IDLE不应响应STOP");
    TEST_ASSERT_EQ(ST_IDLE, fsm.state, "状态应保持");

    /* IDLE状态下不应响应RESET */
    result = fsm_handle(&fsm, EV_RESET);
    TEST_ASSERT(!result, "IDLE不应响应RESET");
}

void test_state_coverage(void) {
    /* 验证所有状态可达 */
    TestFSM fsm;
    fsm_init(&fsm);

    /* 路径: IDLE → ACTIVE → ERROR → IDLE */
    fsm_handle(&fsm, EV_START);
    TEST_ASSERT_EQ(ST_ACTIVE, fsm.state, "ACTIVE可达");

    fsm_handle(&fsm, EV_ERROR);
    TEST_ASSERT_EQ(ST_ERROR, fsm.state, "ERROR可达");

    fsm_handle(&fsm, EV_RESET);
    TEST_ASSERT_EQ(ST_IDLE, fsm.state, "可返回IDLE");
}

/* ========== 主函数 ========== */

int main(void) {
    printf("╔═══════════════════════════════════════╗\n");
    printf("║     状态机单元测试套件                ║\n");
    printf("╚═══════════════════════════════════════╝\n");

    RUN_TEST(test_initial_state);
    RUN_TEST(test_idle_to_active);
    RUN_TEST(test_active_to_idle);
    RUN_TEST(test_active_to_error);
    RUN_TEST(test_error_to_idle);
    RUN_TEST(test_invalid_transitions);
    RUN_TEST(test_state_coverage);

    print_test_report();

    return g_test_stats.failed > 0 ? 1 : 0;
}
```

---

## 三、调试技术

### 3.1 状态追踪

```c
/**
 * @file fsm_debug.h
 * @brief 状态机调试工具
 */

#ifndef FSM_DEBUG_H
#define FSM_DEBUG_H

#include <stdio.h>
#include <time.h>

#define FSM_DEBUG_ENABLED 1

#if FSM_DEBUG_ENABLED

#define FSM_MAX_HISTORY 100

typedef struct {
    uint32_t timestamp;
    const char* state;
    const char* event;
    const char* action;
} FsmHistoryEntry;

typedef struct {
    FsmHistoryEntry entries[FSM_MAX_HISTORY];
    int count;
    int index;
} FsmHistory;

static FsmHistory g_fsm_history = {0};

static inline uint32_t get_timestamp(void) {
    return (uint32_t)clock();
}

#define FSM_LOG(state, event, action) do { \
    if (g_fsm_history.count < FSM_MAX_HISTORY) { \
        g_fsm_history.entries[g_fsm_history.count] = (FsmHistoryEntry){ \
            .timestamp = get_timestamp(), \
            .state = (state), \
            .event = (event), \
            .action = (action) \
        }; \
        g_fsm_history.count++; \
    } \
    printf("[FSM] %10u | %-10s | %-10s | %s\n", \
           get_timestamp(), (state), (event), (action)); \
} while(0)

static inline void fsm_print_history(void) {
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║                    状态机执行历史                           ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║ %-10s | %-10s | %-10s | %s\n", "时间", "状态", "事件", "动作");
    printf("╠════════════════════════════════════════════════════════════╣\n");

    for (int i = 0; i < g_fsm_history.count; i++) {
        FsmHistoryEntry* e = &g_fsm_history.entries[i];
        printf("║ %10u | %-10s | %-10s | %s\n",
               e->timestamp, e->state, e->event, e->action);
    }

    printf("╚════════════════════════════════════════════════════════════╝\n");
}

#else
#define FSM_LOG(state, event, action) ((void)0)
#define fsm_print_history() ((void)0)
#endif

#endif /* FSM_DEBUG_H */
```

### 3.2 断点与断言

```c
/**
 * @brief 状态机断言宏
 */

/* 调试断点 */
#ifdef _WIN32
    #include <intrin.h>
    #define FSM_BREAKPOINT() __debugbreak()
#else
    #include <signal.h>
    #define FSM_BREAKPOINT() raise(SIGTRAP)
#endif

/* 状态断言 */
#define FSM_ASSERT_STATE(fsm, expected) do { \
    if ((fsm)->state != (expected)) { \
        fprintf(stderr, "[FSM断言失败] 期望状态: %s, 实际状态: %s\n", \
                #expected, fsm_state_name((fsm)->state)); \
        FSM_BREAKPOINT(); \
    } \
} while(0)

/* 转换断言 */
#define FSM_ASSERT_TRANS(fsm, event, expected_result) do { \
    bool result = fsm_handle((fsm), (event)); \
    if (result != (expected_result)) { \
        fprintf(stderr, "[FSM断言失败] 事件 %s 应%s触发转换\n", \
                #event, (expected_result) ? "" : "不"); \
        FSM_BREAKPOINT(); \
    } \
} while(0)

/* 不变式检查 */
#define FSM_INVARIANT(fsm, condition, msg) do { \
    if (!(condition)) { \
        fprintf(stderr, "[FSM不变式违反] %s\n", msg); \
        fsm_print_history(); \
        FSM_BREAKPOINT(); \
    } \
} while(0)
```

### 3.3 可视化输出

```c
/**
 * @brief 生成Mermaid状态图
 */

void fsm_generate_mermaid(FSM* fsm) {
    printf("\n```mermaid\n");
    printf("stateDiagram-v2\n");
    printf("    [*] --> %s: 初始\n", fsm->initial_state->name);

    /* 遍历所有状态生成转换 */
    for (int i = 0; i < fsm->state_count; i++) {
        State* s = &fsm->states[i];
        for (int j = 0; j < s->trans_count; j++) {
            Transition* t = &s->transitions[j];
            printf("    %s --> %s: %s\n",
                   s->name, t->target->name, t->event_name);
        }
    }

    printf("```\n");
}

/**
 * @brief 生成PlantUML状态图
 */
void fsm_generate_plantuml(FSM* fsm) {
    printf("\n@startuml\n");
    printf("[*] --> %s\n", fsm->initial_state->name);

    for (int i = 0; i < fsm->state_count; i++) {
        State* s = &fsm->states[i];
        for (int j = 0; j < s->trans_count; j++) {
            Transition* t = &s->transitions[j];
            printf("%s --> %s: %s\n",
                   s->name, t->target->name, t->event_name);
        }
    }

    printf("@enduml\n");
}
```

---

## 四、完整测试案例

```c
/**
 * @file comprehensive_fsm_test.c
 * @brief 综合状态机测试 - 以协议解析器为例
 */

#include "fsm_test_framework.h"
#include "fsm_debug.h"

/* 协议解析器状态机 */
typedef enum {
    PARSER_WAIT_SYNC,
    PARSER_READ_LENGTH,
    PARSER_READ_DATA,
    PARSER_VERIFY_CRC,
    PARSER_COMPLETE,
    PARSER_ERROR
} ParserState;

typedef struct {
    ParserState state;
    uint8_t buffer[256];
    uint8_t length;
    uint8_t received;
    uint8_t crc;
    uint8_t error_count;
} ProtocolParser;

#define SYNC_BYTE 0xAA

void parser_init(ProtocolParser* p) {
    p->state = PARSER_WAIT_SYNC;
    p->length = 0;
    p->received = 0;
    p->crc = 0;
    p->error_count = 0;
}

bool parser_feed(ProtocolParser* p, uint8_t byte) {
    FSM_LOG(parser_state_name(p->state), "BYTE", "");

    switch (p->state) {
        case PARSER_WAIT_SYNC:
            if (byte == SYNC_BYTE) {
                p->state = PARSER_READ_LENGTH;
                FSM_LOG("READ_LENGTH", "SYNC_OK", "");
                return true;
            }
            p->error_count++;
            return false;

        case PARSER_READ_LENGTH:
            if (byte > 0 && byte <= 255) {
                p->length = byte;
                p->received = 0;
                p->crc = byte;
                p->state = PARSER_READ_DATA;
                FSM_LOG("READ_DATA", "LEN_OK", "");
                return true;
            }
            p->state = PARSER_ERROR;
            FSM_LOG("ERROR", "BAD_LENGTH", "");
            return false;

        case PARSER_READ_DATA:
            p->buffer[p->received++] = byte;
            p->crc ^= byte;
            if (p->received >= p->length) {
                p->state = PARSER_VERIFY_CRC;
                FSM_LOG("VERIFY_CRC", "DATA_COMPLETE", "");
            }
            return true;

        case PARSER_VERIFY_CRC:
            if (byte == p->crc) {
                p->state = PARSER_COMPLETE;
                FSM_LOG("COMPLETE", "CRC_OK", "");
            } else {
                p->state = PARSER_ERROR;
                p->error_count++;
                FSM_LOG("ERROR", "CRC_FAIL", "");
            }
            return true;

        case PARSER_COMPLETE:
        case PARSER_ERROR:
            parser_init(p);
            return parser_feed(p, byte);

        default:
            return false;
    }
}

const char* parser_state_name(ParserState s) {
    static const char* names[] = {
        "WAIT_SYNC", "READ_LENGTH", "READ_DATA",
        "VERIFY_CRC", "COMPLETE", "ERROR"
    };
    return (s < 6) ? names[s] : "UNKNOWN";
}

/* ========== 测试用例 ========== */

void test_valid_frame(void) {
    ProtocolParser p;
    parser_init(&p);

    /* 构造有效帧: AA 03 11 22 33 00 (CRC=03^11^22^33=00) */
    uint8_t frame[] = {0xAA, 0x03, 0x11, 0x22, 0x33, 0x00};

    for (size_t i = 0; i < sizeof(frame); i++) {
        parser_feed(&p, frame[i]);
    }

    TEST_ASSERT_EQ(PARSER_COMPLETE, p.state, "应完成解析");
    TEST_ASSERT_EQ(3, p.length, "长度应为3");
    TEST_ASSERT_EQ(0, p.error_count, "不应有错误");
}

void test_invalid_sync(void) {
    ProtocolParser p;
    parser_init(&p);

    bool result = parser_feed(&p, 0xBB);  /* 错误的同步字节 */

    TEST_ASSERT(!result, "错误同步字节应失败");
    TEST_ASSERT_EQ(PARSER_WAIT_SYNC, p.state, "应保持等待同步");
    TEST_ASSERT_EQ(1, p.error_count, "错误计数应为1");
}

void test_invalid_crc(void) {
    ProtocolParser p;
    parser_init(&p);

    /* 构造CRC错误的帧 */
    uint8_t frame[] = {0xAA, 0x03, 0x11, 0x22, 0x33, 0xFF};

    for (size_t i = 0; i < sizeof(frame); i++) {
        parser_feed(&p, frame[i]);
    }

    TEST_ASSERT_EQ(PARSER_ERROR, p.state, "应进入错误状态");
    TEST_ASSERT_EQ(1, p.error_count, "错误计数应为1");
}

void test_state_sequence(void) {
    ProtocolParser p;
    parser_init(&p);

    TEST_ASSERT_EQ(PARSER_WAIT_SYNC, p.state, "初始: WAIT_SYNC");

    parser_feed(&p, 0xAA);
    TEST_ASSERT_EQ(PARSER_READ_LENGTH, p.state, "同步后: READ_LENGTH");

    parser_feed(&p, 0x02);
    TEST_ASSERT_EQ(PARSER_READ_DATA, p.state, "长度后: READ_DATA");

    parser_feed(&p, 0x11);
    TEST_ASSERT_EQ(PARSER_READ_DATA, p.state, "数据中: READ_DATA");

    parser_feed(&p, 0x22);
    TEST_ASSERT_EQ(PARSER_VERIFY_CRC, p.state, "数据完: VERIFY_CRC");
}

void test_continuous_frames(void) {
    ProtocolParser p;
    parser_init(&p);

    /* 第一帧 */
    uint8_t frame1[] = {0xAA, 0x01, 0xAB, 0xAA};
    for (size_t i = 0; i < sizeof(frame1); i++) {
        parser_feed(&p, frame1[i]);
    }
    TEST_ASSERT_EQ(PARSER_COMPLETE, p.state, "第一帧完成");

    /* 第二帧 */
    uint8_t frame2[] = {0xAA, 0x01, 0xCD, 0xCC};
    for (size_t i = 0; i < sizeof(frame2); i++) {
        parser_feed(&p, frame2[i]);
    }
    TEST_ASSERT_EQ(PARSER_COMPLETE, p.state, "第二帧完成");
    TEST_ASSERT_EQ(0xCD, p.buffer[0], "数据正确");
}

/* ========== 性能测试 ========== */

void test_performance(void) {
    ProtocolParser p;
    parser_init(&p);

    clock_t start = clock();

    /* 处理1000帧 */
    for (int frame = 0; frame < 1000; frame++) {
        parser_init(&p);
        uint8_t data[] = {0xAA, 0x02, 0x11, 0x22, 0x33};
        for (size_t i = 0; i < sizeof(data); i++) {
            parser_feed(&p, data[i]);
        }
    }

    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;

    printf("\n  性能: 处理1000帧用时 %.3f秒 (%.0f 帧/秒)\n",
           elapsed, 1000.0 / elapsed);
}

/* ========== 主函数 ========== */

int main(void) {
    printf("╔═══════════════════════════════════════════════════════╗\n");
    printf("║        协议解析器状态机综合测试                        ║\n");
    printf("╚═══════════════════════════════════════════════════════╝\n");

    RUN_TEST(test_valid_frame);
    RUN_TEST(test_invalid_sync);
    RUN_TEST(test_invalid_crc);
    RUN_TEST(test_state_sequence);
    RUN_TEST(test_continuous_frames);

    test_performance();

    printf("\n");
    fsm_print_history();

    print_test_report();

    return g_test_stats.failed > 0 ? 1 : 0;
}
```

---

## 五、调试检查清单

```
┌─────────────────────────────────────────────────────────────┐
│                    状态机调试检查清单                         │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  □ 状态初始化检查                                           │
│    - 初始状态是否正确设置?                                  │
│    - 所有变量是否正确初始化?                                │
│                                                              │
│  □ 转换检查                                                 │
│    - 每个状态转换都有对应的事件?                            │
│    - 无效事件是否被正确处理?                                │
│    - 转换动作是否被执行?                                    │
│                                                              │
│  □ 边界条件检查                                             │
│    - 最小/最大值处理?                                       │
│    - 空指针/空数据检查?                                     │
│    - 溢出检查?                                              │
│                                                              │
│  □ 并发检查(如适用)                                         │
│    - 状态机是否线程安全?                                    │
│    - 事件队列是否正确实现?                                  │
│                                                              │
│  □ 性能检查                                                 │
│    - 状态转换时间可接受?                                    │
│    - 内存使用合理?                                          │
│                                                              │
│  □ 日志检查                                                 │
│    - 关键转换是否记录?                                      │
│    - 错误信息是否清晰?                                      │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

---

## 六、知识检验

### 自测题

1. **为什么状态机测试的覆盖率目标比普通代码更高?**

2. **如何测试一个包含守卫条件的状态转换?**

3. **状态机历史记录对调试有什么帮助?**

4. **在嵌入式环境中，如何平衡调试信息和代码大小?**

### 实践练习

**练习: 为你的状态机添加测试**

选择一个之前实现的状态机：

1. 使用测试框架编写单元测试
2. 添加调试日志
3. 生成状态图可视化
4. 运行性能测试

---

## 七、延伸阅读

- [Switch-Case实现](02_Switch_Case_Implementation.md)
- [查表法实现](03_Table_Driven_Implementation.md)
- [函数指针实现](04_Function_Pointer_Implementation.md)
- [层次状态机](05_Hierarchical_State_Machines.md)

---

**最后更新**: 2026-03-24
**维护者**: C_Lang Knowledge Base Team
**质量等级**: L4 (深化)
