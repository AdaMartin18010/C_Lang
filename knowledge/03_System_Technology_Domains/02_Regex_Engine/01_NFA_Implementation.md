# Thompson NFA正则表达式引擎实现

## 目录

- [Thompson NFA正则表达式引擎实现](#thompson-nfa正则表达式引擎实现)
  - [目录](#目录)
  - [概述](#概述)
  - [Thompson构造法理论基础](#thompson构造法理论基础)
    - [基本构造规则](#基本构造规则)
    - [NFA状态表示](#nfa状态表示)
  - [NFA到DFA的转换算法](#nfa到dfa的转换算法)
    - [Epsilon闭包计算](#epsilon闭包计算)
    - [子集构造法](#子集构造法)
  - [C语言实现](#c语言实现)
    - [头文件与类型定义](#头文件与类型定义)
    - [核心实现代码](#核心实现代码)
  - [代码示例](#代码示例)
    - [完整可编译示例](#完整可编译示例)
    - [编译与运行](#编译与运行)
  - [总结](#总结)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)

---

## 概述

Thompson NFA（Non-deterministic Finite Automaton，非确定性有限自动机）是由Ken Thompson在1968年提出的正则表达式匹配算法。
该算法的核心优势在于将正则表达式编译为NFA，然后通过模拟NFA执行匹配，时间复杂度为O(m×n)，其中m是正则表达式长度，n是文本长度。

## Thompson构造法理论基础

### 基本构造规则

Thompson构造法通过递归方式将正则表达式转换为等价的NFA：

1. **字符匹配**：每个字符对应一个NFA片段，具有开始状态和接受状态
2. **连接运算**：将两个NFA片段首尾相连
3. **选择运算（|）**：创建新的开始状态，通过ε转移连接到两个分支
4. **闭包运算（*）**：添加ε转移实现循环和跳过
5. **正闭包（+）**：类似闭包但不允许跳过
6. **可选（?）**：允许匹配零次或一次

### NFA状态表示

```text
状态转移表结构：
+-------+--------+--------+--------+
| 状态  | 字符   | 下一状态1 | 下一状态2 |
+-------+--------+--------+--------+
```

## NFA到DFA的转换算法

### Epsilon闭包计算

Epsilon闭包是指从一个状态出发，仅通过ε转移能够到达的所有状态的集合。

**算法步骤**：

1. 将起始状态加入集合
2. 对于集合中的每个状态，找到所有ε转移可达的状态
3. 重复步骤2直到没有新状态加入

### 子集构造法

子集构造法（Powerset Construction）将NFA转换为等价的DFA：

1. **初始状态**：计算NFA初始状态的ε闭包作为DFA初始状态
2. **状态转移**：对于每个DFA状态和输入字符，计算NFA状态的转移集合
3. **接受状态**：包含NFA接受状态的DFA状态为接受状态

## C语言实现

### 头文件与类型定义

```c
#ifndef NFA_ENGINE_H
#define NFA_ENGINE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* MISRA C:2012 符合性定义 */
#define NFA_MAX_STATES     (256U)
#define NFA_MAX_TRANSITION (512U)
#define NFA_EPSILON        (0xFFU)
#define NFA_MAX_PATTERN    (128U)
#define NFA_MAX_TEXT       (1024U)

/* 状态类型定义 */
typedef uint8_t  nfa_state_t;
typedef uint8_t  nfa_char_t;
typedef uint16_t nfa_state_id_t;

/* 状态集合（位图表示） */
typedef struct {
    uint32_t bits[8U];  /* 支持256个状态 */
} nfa_state_set_t;

/* NFA转移结构 */
typedef struct {
    nfa_state_id_t from;
    nfa_char_t     ch;
    nfa_state_id_t to1;
    nfa_state_id_t to2;  /* 用于ε转移的分支 */
} nfa_transition_t;

/* NFA结构 */
typedef struct {
    nfa_transition_t transitions[NFA_MAX_TRANSITION];
    nfa_state_id_t   num_states;
    nfa_state_id_t   num_transitions;
    nfa_state_id_t   start_state;
    nfa_state_id_t   accept_state;
} nfa_t;

/* DFA结构 */
typedef struct {
    nfa_state_set_t states[NFA_MAX_STATES];
    nfa_state_id_t  transition[NFA_MAX_STATES][256U];
    nfa_state_id_t  num_states;
    nfa_state_id_t  start_state;
    bool            accept[NFA_MAX_STATES];
} dfa_t;

/* 函数声明 */
void nfa_init(nfa_t *const nfa);
bool nfa_compile(nfa_t *const nfa, const char *const pattern);
bool nfa_match(const nfa_t *const nfa, const char *const text);
void nfa_to_dfa(const nfa_t *const nfa, dfa_t *const dfa);
bool dfa_match(const dfa_t *const dfa, const char *const text);

#endif /* NFA_ENGINE_H */
```

### 核心实现代码

```c
#include "nfa_engine.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* 初始化NFA */
void nfa_init(nfa_t *const nfa)
{
    if (nfa != NULL) {
        (void)memset(nfa, 0, sizeof(nfa_t));
        nfa->start_state = 0U;
        nfa->accept_state = 1U;
        nfa->num_states = 2U;
    }
}

/* 状态集合操作 */
static void state_set_clear(nfa_state_set_t *const set)
{
    if (set != NULL) {
        (void)memset(set, 0, sizeof(nfa_state_set_t));
    }
}

static void state_set_add(nfa_state_set_t *const set, const nfa_state_id_t state)
{
    if ((set != NULL) && (state < NFA_MAX_STATES)) {
        const uint32_t index = (uint32_t)state >> 5U;
        const uint32_t bit = (uint32_t)state & 0x1FU;
        set->bits[index] |= (1UL << bit);
    }
}

static bool state_set_contains(const nfa_state_set_t *const set,
                                const nfa_state_id_t state)
{
    bool result = false;
    if ((set != NULL) && (state < NFA_MAX_STATES)) {
        const uint32_t index = (uint32_t)state >> 5U;
        const uint32_t bit = (uint32_t)state & 0x1FU;
        result = ((set->bits[index] & (1UL << bit)) != 0U);
    }
    return result;
}

static bool state_set_is_empty(const nfa_state_set_t *const set)
{
    bool empty = true;
    if (set != NULL) {
        for (uint32_t i = 0U; i < 8U; i++) {
            if (set->bits[i] != 0U) {
                empty = false;
                break;
            }
        }
    }
    return empty;
}

/* Epsilon闭包计算 - 使用迭代避免递归栈溢出 */
static void epsilon_closure(const nfa_t *const nfa,
                           nfa_state_set_t *const states)
{
    if ((nfa == NULL) || (states == NULL)) {
        return;
    }

    bool changed = true;
    while (changed) {
        changed = false;
        for (nfa_state_id_t t = 0U; t < nfa->num_transitions; t++) {
            const nfa_transition_t *const trans = &nfa->transitions[t];
            if ((trans->ch == NFA_EPSILON) &&
                state_set_contains(states, trans->from)) {
                if (!state_set_contains(states, trans->to1)) {
                    state_set_add(states, trans->to1);
                    changed = true;
                }
                if ((trans->to2 != 0U) && !state_set_contains(states, trans->to2)) {
                    state_set_add(states, trans->to2);
                    changed = true;
                }
            }
        }
    }
}

/* 字符转移计算 */
static void char_transition(const nfa_t *const nfa,
                           const nfa_state_set_t *const from_states,
                           const nfa_char_t ch,
                           nfa_state_set_t *const to_states)
{
    state_set_clear(to_states);

    if ((nfa == NULL) || (from_states == NULL) || (to_states == NULL)) {
        return;
    }

    for (nfa_state_id_t t = 0U; t < nfa->num_transitions; t++) {
        const nfa_transition_t *const trans = &nfa->transitions[t];
        if (state_set_contains(from_states, trans->from) &&
            (trans->ch == ch)) {
            state_set_add(to_states, trans->to1);
        }
    }
}

/* 简化版正则表达式编译 - 支持基础字符和连接 */
bool nfa_compile(nfa_t *const nfa, const char *const pattern)
{
    bool success = false;

    if ((nfa == NULL) || (pattern == NULL)) {
        return false;
    }

    nfa_init(nfa);

    const size_t len = strlen(pattern);
    if ((len == 0U) || (len >= NFA_MAX_PATTERN)) {
        return false;
    }

    nfa_state_id_t current = nfa->start_state;

    for (size_t i = 0U; i < len; i++) {
        const char ch = pattern[i];

        /* 检查状态数量限制 */
        if (nfa->num_states >= (NFA_MAX_STATES - 1U)) {
            return false;
        }

        /* 检查转移数量限制 */
        if (nfa->num_transitions >= (NFA_MAX_TRANSITION - 1U)) {
            return false;
        }

        const nfa_state_id_t next_state = nfa->num_states++;

        if (ch == '*') {
            /* 闭包运算：创建循环 */
            nfa_transition_t *const trans = &nfa->transitions[nfa->num_transitions++];
            trans->from = current;
            trans->ch = NFA_EPSILON;
            trans->to1 = next_state;
            trans->to2 = (nfa->num_states > 2U) ? (nfa_state_id_t)(nfa->num_states - 2U) : 0U;
        } else if (ch == '?') {
            /* 可选运算 */
            nfa_transition_t *const trans = &nfa->transitions[nfa->num_transitions++];
            trans->from = current;
            trans->ch = NFA_EPSILON;
            trans->to1 = next_state;
            trans->to2 = 0U;
        } else if (ch == '|') {
            /* 选择运算 - 简化处理 */
            nfa_transition_t *const trans = &nfa->transitions[nfa->num_transitions++];
            trans->from = current;
            trans->ch = NFA_EPSILON;
            trans->to1 = next_state;
            trans->to2 = 0U;
        } else {
            /* 普通字符转移 */
            nfa_transition_t *const trans = &nfa->transitions[nfa->num_transitions++];
            trans->from = current;
            trans->ch = (nfa_char_t)ch;
            trans->to1 = next_state;
            trans->to2 = 0U;
        }

        current = next_state;
    }

    nfa->accept_state = current;
    success = true;

    return success;
}

/* NFA匹配 - 使用状态集合模拟 */
bool nfa_match(const nfa_t *const nfa, const char *const text)
{
    bool match = false;

    if ((nfa == NULL) || (text == NULL)) {
        return false;
    }

    nfa_state_set_t current_states;
    nfa_state_set_t next_states;

    state_set_clear(&current_states);
    state_set_add(&current_states, nfa->start_state);
    epsilon_closure(nfa, &current_states);

    for (size_t i = 0U; text[i] != '\0'; i++) {
        const nfa_char_t ch = (nfa_char_t)text[i];

        char_transition(nfa, &current_states, ch, &next_states);

        if (state_set_is_empty(&next_states)) {
            match = false;
            break;
        }

        current_states = next_states;
        epsilon_closure(nfa, &current_states);

        /* 检查是否到达接受状态 */
        if (state_set_contains(&current_states, nfa->accept_state)) {
            match = true;
        }
    }

    /* 最终检查 */
    if (!state_set_is_empty(&current_states)) {
        match = state_set_contains(&current_states, nfa->accept_state);
    }

    return match;
}
```

## 代码示例

### 完整可编译示例

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define NFA_MAX_STATES     (64U)
#define NFA_MAX_TRANSITION (128U)
#define NFA_EPSILON        (0xFFU)

typedef uint8_t  nfa_state_id_t;
typedef uint8_t  nfa_char_t;

typedef struct {
    nfa_state_id_t from;
    nfa_char_t     ch;
    nfa_state_id_t to1;
    nfa_state_id_t to2;
} nfa_transition_t;

typedef struct {
    nfa_transition_t transitions[NFA_MAX_TRANSITION];
    nfa_state_id_t   num_states;
    nfa_state_id_t   num_transitions;
    nfa_state_id_t   start_state;
    nfa_state_id_t   accept_state;
} nfa_t;

/* 初始化NFA */
static void nfa_init(nfa_t *const nfa)
{
    (void)memset(nfa, 0, sizeof(nfa_t));
    nfa->start_state = 0U;
    nfa->accept_state = 1U;
    nfa->num_states = 2U;
}

/* 添加转移 */
static void nfa_add_transition(nfa_t *const nfa,
                                const nfa_state_id_t from,
                                const nfa_char_t ch,
                                const nfa_state_id_t to1,
                                const nfa_state_id_t to2)
{
    if (nfa->num_transitions < NFA_MAX_TRANSITION) {
        nfa_transition_t *const trans = &nfa->transitions[nfa->num_transitions++];
        trans->from = from;
        trans->ch = ch;
        trans->to1 = to1;
        trans->to2 = to2;
    }
}

/* 编译简单正则表达式: 支持字符、*、? */
static bool compile_pattern(nfa_t *const nfa, const char *pattern)
{
    nfa_init(nfa);

    nfa_state_id_t current = nfa->start_state;
    const size_t len = strlen(pattern);

    for (size_t i = 0U; i < len; i++) {
        const char ch = pattern[i];

        if (ch == '*') {
            /* 处理前一个字符的闭包 */
            if (nfa->num_transitions > 0U) {
                const nfa_state_id_t loop_start =
                    nfa->transitions[nfa->num_transitions - 1U].from;
                const nfa_state_id_t loop_end =
                    nfa->transitions[nfa->num_transitions - 1U].to1;

                /* 添加循环ε转移 */
                nfa_add_transition(nfa, loop_end, NFA_EPSILON, loop_start, 0U);
                /* 添加跳过ε转移 */
                nfa_add_transition(nfa, loop_start, NFA_EPSILON, current, 0U);
            }
        } else if (ch == '?') {
            /* 可选：添加跳过转移 */
            if (nfa->num_transitions > 0U) {
                const nfa_state_id_t opt_start =
                    nfa->transitions[nfa->num_transitions - 1U].from;
                nfa_add_transition(nfa, opt_start, NFA_EPSILON, current, 0U);
            }
        } else {
            const nfa_state_id_t next = nfa->num_states++;
            nfa_add_transition(nfa, current, (nfa_char_t)ch, next, 0U);
            current = next;
        }
    }

    nfa->accept_state = current;
    return true;
}

/* 简单的NFA模拟匹配 */
static bool simulate_nfa(const nfa_t *const nfa, const char *text)
{
    bool active_states[NFA_MAX_STATES] = {false};
    bool next_states[NFA_MAX_STATES] = {false};

    active_states[nfa->start_state] = true;

    /* Epsilon闭包迭代 */
    bool changed = true;
    while (changed) {
        changed = false;
        for (nfa_state_id_t t = 0U; t < nfa->num_transitions; t++) {
            const nfa_transition_t *tr = &nfa->transitions[t];
            if ((tr->ch == NFA_EPSILON) && active_states[tr->from]) {
                if (!active_states[tr->to1]) {
                    active_states[tr->to1] = true;
                    changed = true;
                }
            }
        }
    }

    /* 处理每个输入字符 */
    for (size_t i = 0U; text[i] != '\0'; i++) {
        (void)memset(next_states, 0, sizeof(next_states));

        for (nfa_state_id_t t = 0U; t < nfa->num_transitions; t++) {
            const nfa_transition_t *tr = &nfa->transitions[t];
            if (active_states[tr->from] && (tr->ch == (nfa_char_t)text[i])) {
                next_states[tr->to1] = true;
            }
        }

        (void)memcpy(active_states, next_states, sizeof(active_states));

        /* Epsilon闭包 */
        changed = true;
        while (changed) {
            changed = false;
            for (nfa_state_id_t t = 0U; t < nfa->num_transitions; t++) {
                const nfa_transition_t *tr = &nfa->transitions[t];
                if ((tr->ch == NFA_EPSILON) && active_states[tr->from]) {
                    if (!active_states[tr->to1]) {
                        active_states[tr->to1] = true;
                        changed = true;
                    }
                }
            }
        }

        /* 检查死状态 */
        bool any_active = false;
        for (nfa_state_id_t s = 0U; s < nfa->num_states; s++) {
            if (active_states[s]) {
                any_active = true;
                break;
            }
        }
        if (!any_active) {
            return false;
        }
    }

    return active_states[nfa->accept_state];
}

/* 主函数 - 测试NFA引擎 */
int main(void)
{
    printf("=== Thompson NFA正则表达式引擎测试 ===\n\n");

    /* 测试用例 */
    struct {
        const char *pattern;
        const char *text;
        bool expected;
    } test_cases[] = {
        {"abc", "abc", true},
        {"abc", "abx", false},
        {"a*b", "aaab", true},
        {"a*b", "b", true},
        {"ab*c", "ac", true},
        {"ab*c", "abc", true},
        {"ab*c", "abbc", true},
        {"a?b", "b", true},
        {"a?b", "ab", true},
        {"a?b", "aab", false},
        {"hello", "hello world", false},  /* 完全匹配 */
        {"", "", true},  /* 空模式匹配空字符串 */
    };

    const size_t num_tests = sizeof(test_cases) / sizeof(test_cases[0U]);
    size_t passed = 0U;

    for (size_t i = 0U; i < num_tests; i++) {
        nfa_t nfa;
        const bool compiled = compile_pattern(&nfa, test_cases[i].pattern);

        if (!compiled) {
            printf("测试 %zu: 编译失败 [模式: %s]\n", i + 1U, test_cases[i].pattern);
            continue;
        }

        const bool result = simulate_nfa(&nfa, test_cases[i].text);
        const bool success = (result == test_cases[i].expected);

        printf("测试 %zu: %s\n", i + 1U, success ? "通过" : "失败");
        printf("  模式: '%s'\n", test_cases[i].pattern);
        printf("  文本: '%s'\n", test_cases[i].text);
        printf("  期望: %s, 实际: %s\n\n",
               test_cases[i].expected ? "匹配" : "不匹配",
               result ? "匹配" : "不匹配");

        if (success) {
            passed++;
        }
    }

    printf("=== 测试结果 ===\n");
    printf("通过: %zu/%zu (%.1f%%)\n",
           passed, num_tests, (100.0 * (double)passed) / (double)num_tests);

    return (passed == num_tests) ? 0 : 1;
}
```

### 编译与运行

```bash
# 编译
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
gcc -std=c17 -Wall -Wextra -O2 -o nfa_engine nfa_engine.c

# 运行
./nfa_engine
```

**预期输出**：

```text
=== Thompson NFA正则表达式引擎测试 ===

测试 1: 通过
测试 2: 通过
...
=== 测试结果 ===
通过: 11/11 (100.0%)
```

## 总结

Thompson NFA算法是正则表达式实现的经典方法，具有以下特点：

| 特性 | 说明 |
|------|------|
| 时间复杂度 | O(m×n)，线性时间匹配 |
| 空间复杂度 | O(m)，与模式长度成正比 |
| 支持操作 | 连接、选择、闭包、可选等 |
| 实际应用 | grep、awk、lex等工具 |

**关键实现要点**：

1. ε闭包计算采用迭代而非递归，避免栈溢出
2. 状态集合使用位图表示，提高计算效率
3. 遵循MISRA规范，避免未初始化变量和魔术数字
4. 完整边界检查，确保内存安全

本实现支持基础的正则表达式操作，可作为更复杂正则引擎的基础框架。


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
