# 正则表达式引擎 (Regex Engine)

## 目录

- [正则表达式引擎 (Regex Engine)](#正则表达式引擎-regex-engine)
  - [目录](#目录)
  - [概述](#概述)
    - [正则表达式引擎类型](#正则表达式引擎类型)
  - [NFA与DFA](#nfa与dfa)
    - [自动机基础](#自动机基础)
    - [NFA模拟匹配](#nfa模拟匹配)
  - [Thompson构造法](#thompson构造法)
    - [构造规则](#构造规则)
    - [Thompson构造实现](#thompson构造实现)
  - [正则匹配算法](#正则匹配算法)
    - [NFA回溯算法](#nfa回溯算法)
  - [回溯优化](#回溯优化)
    - [常见优化技术](#常见优化技术)
    - [RE2优化策略](#re2优化策略)
  - [实践实现](#实践实现)
    - [完整正则引擎实现](#完整正则引擎实现)
  - [参考资源](#参考资源)
    - [经典文献](#经典文献)
    - [开源实现](#开源实现)

---

## 概述

正则表达式（Regular Expression）是一种描述字符串模式的强大工具，广泛应用于文本搜索、验证、替换等场景。
正则表达式引擎负责将模式编译为可执行的匹配程序。

### 正则表达式引擎类型

```text
┌─────────────────────────────────────────────────────────────────────┐
│                    正则引擎类型对比                                  │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  【DFA引擎】 (Deterministic Finite Automaton)                        │
│                                                                     │
│  特点:                                                              │
│  ✓ 匹配时间与输入长度线性相关 O(n)                                   │
│  ✓ 保证最坏情况下性能                                               │
│  ✓ 无回溯，无栈溢出风险                                             │
│  ✗ 不支持反向引用和断言                                             │
│  ✗ 不支持非贪婪匹配                                                 │
│  ✗ 内存消耗可能较大（状态爆炸）                                      │
│                                                                     │
│  代表:                                                              │
│  • grep (大多数实现)                                                 │
│  • awk                                                              │
│  • RE2 (Google)                                                     │
│                                                                     │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  【NFA引擎】 (Nondeterministic Finite Automaton)                     │
│                                                                     │
│  特点:                                                              │
│  ✓ 支持所有正则特性（反向引用、断言等）                               │
│  ✓ 支持非贪婪匹配                                                   │
│  ✓ 实现相对简单                                                     │
│  ✗ 最坏情况指数时间复杂度 O(2^n)                                     │
│  ✗ 回溯可能导致性能问题和栈溢出                                       │
│                                                                     │
│  代表:                                                              │
│  • PCRE (Perl Compatible Regular Expressions)                       │
│  • Python re                                                        │
│  • Java Pattern                                                     │
│  • JavaScript RegExp                                                │
│                                                                     │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  【混合引擎】                                                        │
│                                                                     │
│  策略: 先用DFA快速匹配，复杂特性用NFA                                  │
│                                                                     │
│  代表:                                                              │
│  • Tcl regexp                                                       │
│  • GNU grep (部分实现)                                              │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

---

## NFA与DFA

### 自动机基础

```text
┌─────────────────────────────────────────────────────────────────────┐
│              有限自动机 (Finite Automaton)                           │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  【NFA - 非确定有限自动机】                                          │
│                                                                     │
│  特点:                                                              │
│  • 同一条边可由多个字符触发                                         │
│  • 存在ε转移（空转移，不消耗字符）                                   │
│  • 多个活跃状态同时存在                                             │
│                                                                     │
│  示例: 正则表达式 "a*b"                                             │
│                                                                     │
│         ε     a     ε                                               │
│      ┌───┐  ┌───┐  ┌───┐                                           │
│   → (q0)─→─(q1)─→─(q2)                                             │
│      │ ↑   └───┘   │  │                                            │
│      │ └───────────┘  │ b                                          │
│      └────────────────┘                                             │
│                       ↓                                             │
│                      (( ))                                          │
│                                                                     │
│  q0: 起始状态  q1: 匹配a  q2: 接受状态                               │
│                                                                     │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  【DFA - 确定有限自动机】                                            │
│                                                                     │
│  特点:                                                              │
│  • 每条边由唯一字符触发                                             │
│  • 无ε转移                                                          │
│  • 每个时刻只有一个活跃状态                                         │
│                                                                     │
│  示例: 正则表达式 "a*b" (NFA转DFA后)                                │
│                                                                     │
│                    a                                                │
│                 ┌────┐                                              │
│                 │    │                                              │
│                 ↓    │                                              │
│      ┌───┐  a  ┌───┐ │ b  ┌───┐                                    │
│   → (A) ───→ (B) ←──┘──→ (C)                                       │
│      │                (接受)                                        │
│      │ b                                                            │
│      ↓                                                              │
│     (D)  ← 死状态 (匹配失败)                                        │
│                                                                     │
│  状态说明:                                                          │
│  A = {q0,q2}  (等价于NFA的初始状态闭包)                             │
│  B = {q1,q2}  (读了至少一个a)                                       │
│  C = 接受状态                                                       │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

### NFA模拟匹配

```c
/**
 * @file nfa_simulation.c
 * @brief NFA模拟匹配实现
 *
 * 使用子集构造法模拟NFA运行
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_STATES 64
#define MAX_CHARS 128
#define EPSILON 0  /* ε转移用字符0表示 */

/* NFA状态 */
typedef struct State State;
struct State {
    int id;
    State *next[MAX_CHARS];  /* 转移表 */
    int num_next[MAX_CHARS]; /* 每个字符的转移数量 */
    bool is_accept;
};

/* NFA */
typedef struct {
    State *start;
    State *accept;
    State states[MAX_STATES];
    int num_states;
} NFA;

/* 创建新状态 */
State* new_state(NFA *nfa) {
    State *s = &nfa->states[nfa->num_states++];
    s->id = nfa->num_states - 1;
    s->is_accept = false;
    memset(s->num_next, 0, sizeof(s->num_next));
    return s;
}

/* 添加转移 */
void add_transition(State *from, char c, State *to) {
    unsigned char uc = (unsigned char)c;
    from->next[uc][from->num_next[uc]++] = to;
}

/* 计算ε闭包 */
void epsilon_closure(State *s, bool *closure, int num_states) {
    /* BFS求ε闭包 */
    State *stack[MAX_STATES];
    int top = 0;

    stack[top++] = s;
    closure[s->id] = true;

    while (top > 0) {
        State *cur = stack[--top];

        /* 遍历所有ε转移 */
        for (int i = 0; i < cur->num_next[EPSILON]; i++) {
            State *next = cur->next[EPSILON][i];
            if (!closure[next->id]) {
                closure[next->id] = true;
                stack[top++] = next;
            }
        }
    }
}

/* NFA匹配 */
bool nfa_match(NFA *nfa, const char *input) {
    bool current[MAX_STATES] = {false};
    bool next[MAX_STATES];

    /* 计算初始状态的ε闭包 */
    epsilon_closure(nfa->start, current, nfa->num_states);

    for (const char *p = input; *p != '\0'; p++) {
        memset(next, 0, sizeof(next));
        unsigned char c = (unsigned char)*p;

        /* 从当前所有活跃状态转移 */
        for (int i = 0; i < nfa->num_states; i++) {
            if (current[i]) {
                State *s = &nfa->states[i];
                for (int j = 0; j < s->num_next[c]; j++) {
                    State *ns = s->next[c][j];
                    epsilon_closure(ns, next, nfa->num_states);
                }
            }
        }

        memcpy(current, next, sizeof(current));

        /* 检查是否还有活跃状态 */
        bool has_active = false;
        for (int i = 0; i < nfa->num_states; i++) {
            if (current[i]) {
                has_active = true;
                break;
            }
        }
        if (!has_active) return false;
    }

    /* 检查是否到达接受状态 */
    for (int i = 0; i < nfa->num_states; i++) {
        if (current[i] && nfa->states[i].is_accept) {
            return true;
        }
    }
    return false;
}

/* 构建简单NFA: a*b */
void build_nfa_a_star_b(NFA *nfa) {
    nfa->num_states = 0;

    State *q0 = new_state(nfa);
    State *q1 = new_state(nfa);
    State *q2 = new_state(nfa);

    nfa->start = q0;
    nfa->accept = q2;
    q2->is_accept = true;

    /* q0 --ε--> q1 */
    add_transition(q0, EPSILON, q1);
    /* q0 --ε--> q2 (a*可以匹配0个a) */
    add_transition(q0, EPSILON, q2);
    /* q1 --a--> q1 (循环) */
    add_transition(q1, 'a', q1);
    /* q1 --ε--> q2 */
    add_transition(q1, EPSILON, q2);
    /* q2 --b--> q2 */
    add_transition(q2, 'b', q2);
}

int main(void) {
    NFA nfa;
    build_nfa_a_star_b(&nfa);

    printf("NFA for pattern: a*b\n\n");

    const char *tests[] = {"b", "ab", "aab", "aaab", "a", "c", ""};
    int num_tests = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < num_tests; i++) {
        bool result = nfa_match(&nfa, tests[i]);
        printf("Match '%s': %s\n", tests[i], result ? "YES" : "NO");
    }

    return 0;
}
```

---

## Thompson构造法

### 构造规则

```
┌─────────────────────────────────────────────────────────────────────┐
│              Thompson NFA 构造规则                                   │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  【基本单元】                                                        │
│                                                                     │
│  空串 ε:                    字符 a:                                 │
│      ┌───┐                    ┌───┐  a   ┌───┐                     │
│   → (   )─→                → (   )────→(   )─→                     │
│      └───┘                    └───┘      └───┘                     │
│                                                                     │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  【组合规则】                                                        │
│                                                                     │
│  1. 连接 AB:                                                        │
│                                                                     │
│        N(A)    N(B)                                                 │
│     → ( ... )─→( ... )─→                                           │
│                                                                     │
│  2. 选择 A|B:                                                       │
│                                                                     │
│              ┌─→ N(A) ─┐                                            │
│     → ( )─→─→┤         ├─→─→( )─→                                   │
│              └─→ N(B) ─┘                                            │
│                                                                     │
│  3. 闭包 A*:                                                        │
│                                                                     │
│              ┌──────────┐                                           │
│              ↓          │ ε                                         │
│     → ( )─→ N(A) ─→( )─→                                           │
│         │      ↑          │                                         │
│         └──────┴──────────┘                                         │
│              ε                                                      │
│                                                                     │
│  4. 正闭包 A+ = AA*                                                 │
│                                                                     │
│  5. 可选 A? = A|ε                                                   │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

### Thompson构造实现

```c
/**
 * @file thompson.c
 * @brief Thompson NFA构造法实现
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_FRAGMENTS 256
#define MAX_STATES 512

typedef struct State State;
struct State {
    int id;
    char c;           /* 转移字符，0表示ε，256表示分裂 */
    State *out;
    State *out1;      /* 用于分裂状态 */
    int last_list;    /* 用于匹配时的标记 */
};

typedef struct {
    State *start;
    State **out;      /* 指向未连接转移的指针列表 */
    int num_out;
} Frag;

typedef struct {
    State states[MAX_STATES];
    int nstate;
} StateList;

State* state(StateList *list, char c, State *out, State *out1) {
    State *s = &list->states[list->nstate++];
    s->id = list->nstate - 1;
    s->c = c;
    s->out = out;
    s->out1 = out1;
    s->last_list = 0;
    return s;
}

/* 创建片段 */
Frag frag(State *start, State **out, int num_out) {
    Frag f = {start, out, num_out};
    return f;
}

/* 连接两个片段 */
Frag concat(Frag f1, Frag f2) {
    /* 将f1的所有未连接指向f2.start */
    for (int i = 0; i < f1.num_out; i++) {
        *f1.out[i] = *f2.start;
    }
    return frag(f1.start, f2.out, f2.num_out);
}

/* 创建选择 */
Frag alternate(StateList *list, Frag f1, Frag f2) {
    State *s = state(list, 256, f1.start, f2.start);  /* 256 = 分裂 */

    /* 合并未连接列表 */
    State **out = malloc((f1.num_out + f2.num_out) * sizeof(State*));
    memcpy(out, f1.out, f1.num_out * sizeof(State*));
    memcpy(out + f1.num_out, f2.out, f2.num_out * sizeof(State*));

    return frag(s, out, f1.num_out + f2.num_out);
}

/* 创建闭包 A* */
Frag star(StateList *list, Frag f) {
    State *s = state(list, 256, f.start, NULL);  /* 分裂状态 */

    /* 将f的所有未连接指向s（循环） */
    for (int i = 0; i < f.num_out; i++) {
        *f.out[i] = *s;
    }

    /* s.out1是新的未连接（匹配0次的情况） */
    State **out = malloc(sizeof(State*));
    out[0] = &s->out1;

    return frag(s, out, 1);
}

/* 解析简单正则表达式并构建NFA (简化版) */
State* parse_regex(StateList *list, const char *pattern);

/* 简化版：只支持基本字符和* */
State* parse_simple(StateList *list, const char *pattern, int *pos) {
    Frag stack[MAX_FRAGMENTS];
    int stack_top = 0;

    while (pattern[*pos] != '\0' && pattern[*pos] != ')') {
        char c = pattern[*pos];

        if (c == '*') {
            /* 将栈顶应用闭包 */
            if (stack_top > 0) {
                stack[stack_top - 1] = star(list, stack[stack_top - 1]);
            }
            (*pos)++;
        } else if (c == '|') {
            /* 处理选择（简化：假设只处理下一个字符） */
            (*pos)++;
            Frag right = parse_simple(list, pattern, pos);
            if (stack_top > 0) {
                stack[stack_top - 1] = alternate(list, stack[stack_top - 1], right);
            }
        } else {
            /* 普通字符 */
            State *s = state(list, c, NULL, NULL);
            State **out = malloc(sizeof(State*));
            out[0] = &s->out;
            stack[stack_top++] = frag(s, out, 1);
            (*pos)++;
        }
    }

    /* 连接所有片段 */
    if (stack_top == 0) {
        /* 空串 */
        State *s = state(list, 0, NULL, NULL);
        State **out = malloc(sizeof(State*));
        out[0] = &s->out;
        return frag(s, out, 1).start;
    }

    Frag result = stack[0];
    for (int i = 1; i < stack_top; i++) {
        result = concat(result, stack[i]);
    }

    /* 添加接受状态 */
    State *accept = state(list, 257, NULL, NULL);  /* 257 = 接受 */
    for (int i = 0; i < result.num_out; i++) {
        *result.out[i] = *accept;
    }

    return result.start;
}

/* 匹配函数 */
#define LIST_SIZE 64

int list_id = 0;

void add_state(State **list, int *n, State *s) {
    if (s == NULL || s->last_list == list_id) return;
    s->last_list = list_id;
    if (s->c == 256) {  /* 分裂状态 */
        add_state(list, n, s->out);
        add_state(list, n, s->out1);
    } else {
        list[(*n)++] = s;
    }
}

bool match(State *start, const char *input) {
    State *current[LIST_SIZE], *next[LIST_SIZE];
    int n_cur = 0, n_next = 0;

    list_id++;
    add_state(current, &n_cur, start);

    for (const char *p = input; ; p++) {
        unsigned char c = *p;

        if (n_cur == 0) return false;

        list_id++;
        n_next = 0;

        for (int i = 0; i < n_cur; i++) {
            State *s = current[i];
            if (s->c == c || (s->c == 257 && c == '\0')) {
                add_state(next, &n_next, s->out);
            }
        }

        /* 检查是否接受 */
        if (c == '\0') {
            for (int i = 0; i < n_cur; i++) {
                if (current[i]->c == 257) return true;
            }
            return false;
        }

        /* 交换current和next */
        memcpy(current, next, n_next * sizeof(State*));
        n_cur = n_next;
    }
}

int main(void) {
    StateList list = {.nstate = 0};

    /* 测试模式 "ab*c" */
    int pos = 0;
    State *start = parse_simple(&list, "ab*c", &pos);

    printf("Pattern: ab*c\n\n");

    const char *tests[] = {"ac", "abc", "abbc", "abbbc", "ab", "a"};
    int n = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < n; i++) {
        printf("Match '%s': %s\n", tests[i], match(start, tests[i]) ? "YES" : "NO");
    }

    return 0;
}
```

---

## 正则匹配算法

### NFA回溯算法

```c
/**
 * @file backtrack.c
 * @brief NFA回溯匹配算法
 *
 * 支持捕获组和反向引用
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_GROUPS 10
#define MAX_STATES 256

typedef struct Inst Inst;
struct Inst {
    enum { CHAR, SPLIT, JMP, MATCH, SAVE } opcode;
    char c;              /* 对于CHAR */
    Inst *x, *y;         /* 对于SPLIT/JMP */
    int n;               /* 对于SAVE: 保存的组号 */
};

typedef struct {
    const char *sp;      /* 字符串位置 */
    Inst *pc;            /* 程序计数器 */
} Thread;

/* 简单指令创建 */
Inst* inst_char(char c) {
    Inst *i = malloc(sizeof(Inst));
    i->opcode = CHAR;
    i->c = c;
    return i;
}

Inst* inst_split(Inst *x, Inst *y) {
    Inst *i = malloc(sizeof(Inst));
    i->opcode = SPLIT;
    i->x = x;
    i->y = y;
    return i;
}

Inst* inst_jmp(Inst *x) {
    Inst *i = malloc(sizeof(Inst));
    i->opcode = JMP;
    i->x = x;
    return i;
}

Inst* inst_save(int n) {
    Inst *i = malloc(sizeof(Inst));
    i->opcode = SAVE;
    i->n = n;
    return i;
}

Inst* inst_match(void) {
    Inst *i = malloc(sizeof(Inst));
    i->opcode = MATCH;
    return i;
}

/* 匹配上下文 */
typedef struct {
    const char *input;
    const char *groups[MAX_GROUPS * 2];  /* 起始/结束位置 */
    int matched;
} MatchContext;

/* 递归回溯匹配 */
bool backtrack(MatchContext *ctx, Inst *pc, const char *sp) {
    for (;;) {
        switch (pc->opcode) {
            case CHAR:
                if (*sp != pc->c) return false;
                pc++;
                sp++;
                break;

            case SPLIT:
                /* 尝试两个分支 */
                if (backtrack(ctx, pc->x, sp)) return true;
                pc = pc->y;
                break;

            case JMP:
                pc = pc->x;
                break;

            case SAVE:
                {
                    const char *old = ctx->groups[pc->n];
                    ctx->groups[pc->n] = sp;
                    if (backtrack(ctx, pc + 1, sp)) return true;
                    ctx->groups[pc->n] = old;  /* 回溯 */
                    return false;
                }

            case MATCH:
                ctx->matched = 1;
                return true;
        }
    }
}

/* 构建简单程序: (a|b)*c */
Inst* build_program(void) {
    /* 简化：直接构建 */
    Inst *match = inst_match();

    Inst *c = inst_char('c');
    c->x = match;

    Inst *a = inst_char('a');
    Inst *b = inst_char('b');

    Inst *split = inst_split(a, b);
    a->x = split;
    b->x = split;

    Inst *start = inst_split(c, split);
    split->x = c;

    return start;
}

int main(void) {
    Inst *prog = build_program();

    printf("Pattern: (a|b)*c\n\n");

    const char *tests[] = {"c", "ac", "bc", "abc", "abac", "d"};
    int n = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < n; i++) {
        MatchContext ctx = {0};
        ctx.input = tests[i];
        bool result = backtrack(&ctx, prog, tests[i]);
        printf("Match '%s': %s\n", tests[i], result ? "YES" : "NO");
    }

    return 0;
}
```

---

## 回溯优化

### 常见优化技术

```
┌─────────────────────────────────────────────────────────────────────┐
│                    正则引擎优化技术                                  │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  【1. 预过滤优化】                                                   │
│                                                                     │
│  模式: /foo.*bar/                                                   │
│  优化: 先用Boyer-Moore查找"foo"，再尝试完整匹配                      │
│                                                                     │
│  字符串: "xxxxxxxxfooxxxxxbarxxx"                                   │
│            ↑快速定位                                                │
│                                                                     │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  【2. 原子组与占有量词】                                             │
│                                                                     │
│  问题模式: (a+)+b    对 "aaaaaaaaaaaaaaaaaaaaaac" 灾难性回溯       │
│                                                                     │
│  解决方案:                                                           │
│  • 原子组 (?>a+)+b  - 一旦匹配，不回溯                              │
│  • 占有量词 a++b    - 量词不释放已匹配字符                          │
│                                                                     │
│  传统回溯:                                                           │
│  a+ 匹配 n个a，然后失败，回溯                                       │
│  a+ 匹配 n-1个a，然后失败，回溯                                     │
│  ... 指数级复杂度                                                    │
│                                                                     │
│  原子组:                                                             │
│  a+ 匹配尽可能多的a，不回溯，直接失败                                │
│                                                                     │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  【3. 递归匹配优化】                                                  │
│                                                                     │
│  问题: 嵌套括号匹配 ()                                              │
│                                                                     │
│  PCRE递归:  \( ( (?>[^()]+) | (?R) )* \)                            │
│                                                                     │
│  (?R) 表示递归调用整个模式                                          │
│                                                                     │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  【4. JIT编译优化】                                                   │
│                                                                     │
│  热点正则表达式编译为机器码：                                        │
│                                                                     │
│  字节码: CHAR 'a'                                                   │
│  JIT:   cmp byte ptr [rsi], 'a'                                     │
│         jne mismatch                                                │
│         inc rsi                                                     │
│                                                                     │
│  PCRE2支持JIT，可提升10倍性能                                        │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

### RE2优化策略

```c
/**
 * @file re2_strategy.c
 * @brief RE2风格的正则匹配策略
 *
 * RE2保证最坏情况下线性时间复杂度
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

/* RE2核心策略:
 * 1. 使用DFA进行快速匹配
 * 2. DFA状态数限制，避免内存爆炸
 * 3. 不支持反向引用，避免NP完全问题
 * 4. One-pass优化：某些模式可一次遍历完成
 */

/* One-pass NFA检测 */
bool is_one_pass(const char *pattern) {
    /* 以下条件满足时为one-pass:
     * - 无回溯需求
     * - 每个位置的选择是确定的
     * - 无复杂的贪婪/非贪婪交替
     */

    /* 简化检测：不包含以下模式 */
    if (strstr(pattern, "(?") != NULL) return false;  /* 无扩展语法 */
    if (strstr(pattern, "*?") != NULL) return false; /* 无非贪婪 */
    if (strstr(pattern, "|") != NULL) {
        /* 有选择操作符，需要更复杂分析 */
        /* 简化：假设不是one-pass */
        return false;
    }

    return true;
}

/* Bitap算法（Baeza-Yates-Gonnet）- 近似匹配 */
/* 适合短模式，位并行实现 */

#include <stdint.h>

#define ALPHABET_SIZE 256

/* Bitap匹配 */
int bitap_search(const char *text, const char *pattern, int max_errors) {
    int m = strlen(pattern);
    if (m > 31) return -1;  /* 限制模式长度 */

    /* 构建掩码表 */
    uint32_t mask[ALPHABET_SIZE];
    for (int i = 0; i < ALPHABET_SIZE; i++) mask[i] = ~0;

    for (int i = 0; i < m; i++) {
        mask[(unsigned char)pattern[i]] &= ~(1 << i);
    }

    uint32_t R[max_errors + 1];
    for (int i = 0; i <= max_errors; i++) R[i] = ~0;
    R[0] &= ~1;  /* 初始状态 */

    for (int pos = 0; text[pos] != '\0'; pos++) {
        uint32_t old_R = R[0];
        R[0] = (R[0] | mask[(unsigned char)text[pos]]) << 1;

        for (int e = 1; e <= max_errors; e++) {
            uint32_t tmp = R[e];
            R[e] = ((R[e] | mask[(unsigned char)text[pos]]) << 1)
                   | old_R
                   | (old_R << 1)
                   | (R[e-1] << 1);
            old_R = tmp;
        }

        /* 检查匹配 */
        if (!(R[max_errors] & (1 << m))) {
            return pos - m + 1;
        }
    }

    return -1;
}

/* DFA缓存策略 */
#define DFA_CACHE_SIZE 1024

typedef struct {
    int from_state;
    char input;
    int to_state;
} DFATransition;

static DFATransition dfa_cache[DFA_CACHE_SIZE];
static int cache_size = 0;

/* 简单的DFA转移缓存 */
int dfa_transition(int state, char c, int (*nfa_step)(int, char)) {
    /* 先在缓存中查找 */
    for (int i = 0; i < cache_size; i++) {
        if (dfa_cache[i].from_state == state &&
            dfa_cache[i].input == c) {
            return dfa_cache[i].to_state;
        }
    }

    /* 计算新状态 */
    int new_state = nfa_step(state, c);

    /* 加入缓存 */
    if (cache_size < DFA_CACHE_SIZE) {
        dfa_cache[cache_size++] = (DFATransition){state, c, new_state};
    }

    return new_state;
}

/* RE2风格匹配接口 */
bool re2_match(const char *pattern, const char *text) {
    printf("RE2 matching '%s' against '%s'\n", pattern, text);

    /* 1. 解析模式 */
    /* 2. 构建NFA */
    /* 3. 使用DFA模拟（带缓存）*/
    /* 4. 返回结果 */

    if (is_one_pass(pattern)) {
        printf("  Using one-pass optimization\n");
    }

    /* 简化：使用bitap演示 */
    if (strlen(pattern) <= 31) {
        int pos = bitap_search(text, pattern, 0);
        return pos >= 0;
    }

    return false;
}

int main(void) {
    printf("=== RE2-Style Regex Engine Demo ===\n\n");

    printf("One-pass detection:\n");
    printf("  'abc': %s\n", is_one_pass("abc") ? "yes" : "no");
    printf("  'a|b': %s\n", is_one_pass("a|b") ? "yes" : "no");

    printf("\nBitap approximate search (1 error allowed):\n");
    int pos = bitap_search("hello world", "hallo", 1);
    printf("  'hallo' in 'hello world' (k=1): pos=%d\n", pos);

    return 0;
}
```

---

## 实践实现

### 完整正则引擎实现

```c
/**
 * @file regex_engine.c
 * @brief 简化但完整的正则引擎
 *
 * 支持: . * + ? | () [^a-z] \d \w
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

/* ============ 词法分析器 ============ */

typedef enum {
    TOK_CHAR, TOK_DOT, TOK_STAR, TOK_PLUS, TOK_QUESTION,
    TOK_PIPE, TOK_LPAREN, TOK_RPAREN, TOK_LBRACKET, TOK_RBRACKET,
    TOK_CHARSET, TOK_EOF
} TokenType;

typedef struct {
    TokenType type;
    char c;              /* 对于TOK_CHAR */
    char *charset;       /* 对于TOK_CHARSET */
} Token;

typedef struct {
    const char *input;
    int pos;
} Lexer;

void lexer_init(Lexer *lex, const char *input) {
    lex->input = input;
    lex->pos = 0;
}

/* 解析字符集 [abc] 或 [^abc] */
char* parse_charset(Lexer *lex) {
    bool negated = false;
    if (lex->input[lex->pos] == '^') {
        negated = true;
        lex->pos++;
    }

    char *set = malloc(256);
    memset(set, negated ? 1 : 0, 256);

    while (lex->input[lex->pos] != ']' && lex->input[lex->pos] != '\0') {
        char c = lex->input[lex->pos++];
        set[(unsigned char)c] = negated ? 0 : 1;
    }

    if (lex->input[lex->pos] == ']') lex->pos++;
    return set;
}

Token lexer_next(Lexer *lex) {
    Token tok = {TOK_EOF, 0, NULL};
    char c = lex->input[lex->pos];

    if (c == '\0') return tok;
    lex->pos++;

    switch (c) {
        case '.': tok.type = TOK_DOT; break;
        case '*': tok.type = TOK_STAR; break;
        case '+': tok.type = TOK_PLUS; break;
        case '?': tok.type = TOK_QUESTION; break;
        case '|': tok.type = TOK_PIPE; break;
        case '(': tok.type = TOK_LPAREN; break;
        case ')': tok.type = TOK_RPAREN; break;
        case '[':
            tok.type = TOK_CHARSET;
            tok.charset = parse_charset(lex);
            break;
        case '\\':
            c = lex->input[lex->pos++];
            switch (c) {
                case 'd':
                    tok.type = TOK_CHARSET;
                    tok.charset = malloc(256);
                    memset(tok.charset, 0, 256);
                    for (int i = '0'; i <= '9'; i++) tok.charset[i] = 1;
                    break;
                case 'w':
                    tok.type = TOK_CHARSET;
                    tok.charset = malloc(256);
                    memset(tok.charset, 0, 256);
                    for (int i = 'a'; i <= 'z'; i++) tok.charset[i] = 1;
                    for (int i = 'A'; i <= 'Z'; i++) tok.charset[i] = 1;
                    for (int i = '0'; i <= '9'; i++) tok.charset[i] = 1;
                    tok.charset['_'] = 1;
                    break;
                default:
                    tok.type = TOK_CHAR;
                    tok.c = c;
            }
            break;
        default:
            tok.type = TOK_CHAR;
            tok.c = c;
    }

    return tok;
}

/* ============ 语法分析 & NFA构建 ============ */

typedef struct Inst Inst;
struct Inst {
    enum { I_CHAR, I_CHARSET, I_SPLIT, I_JMP, I_MATCH, I_SAVE } op;
    char c;
    char *charset;
    Inst *x, *y;
    int save_id;
};

typedef struct {
    Inst *start;
    Inst **out;
    int nout;
} Frag;

typedef struct {
    Inst *insts;
    int ninst;
    int maxinst;
    Token current;
    Lexer *lexer;
} Parser;

void parser_init(Parser *p, Lexer *lex) {
    p->insts = malloc(256 * sizeof(Inst));
    p->ninst = 0;
    p->maxinst = 256;
    p->lexer = lex;
    p->current = lexer_next(lex);
}

Inst* emit(Parser *p, Inst i) {
    if (p->ninst >= p->maxinst) {
        p->maxinst *= 2;
        p->insts = realloc(p->insts, p->maxinst * sizeof(Inst));
    }
    p->insts[p->ninst] = i;
    return &p->insts[p->ninst++];
}

void advance(Parser *p) {
    p->current = lexer_next(p->lexer);
}

Frag make_frag(Inst *start, Inst **out, int nout) {
    Frag f = {start, out, nout};
    return f;
}

Frag concat(Frag f1, Frag f2) {
    for (int i = 0; i < f1.nout; i++) {
        *f1.out[i] = *f2.start;
    }
    free(f1.out);
    return make_frag(f1.start, f2.out, f2.nout);
}

Frag alternate(Parser *p, Frag f1, Frag f2) {
    Inst *split = emit(p, (Inst){I_SPLIT, 0, NULL, f1.start, f2.start, 0});
    Inst **out = malloc((f1.nout + f2.nout) * sizeof(Inst*));
    memcpy(out, f1.out, f1.nout * sizeof(Inst*));
    memcpy(out + f1.nout, f2.out, f2.nout * sizeof(Inst*));
    free(f1.out);
    free(f2.out);
    return make_frag(split, out, f1.nout + f2.nout);
}

Frag star(Parser *p, Frag f) {
    Inst *split = emit(p, (Inst){I_SPLIT, 0, NULL, NULL, NULL, 0});
    Inst *jump = emit(p, (Inst){I_JMP, 0, NULL, split, NULL, 0});

    for (int i = 0; i < f.nout; i++) {
        *f.out[i] = *split;
    }
    free(f.out);

    split->x = f.start;
    split->y = jump;

    Inst **out = malloc(sizeof(Inst*));
    out[0] = jump;
    return make_frag(split, out, 1);
}

Frag plus(Parser *p, Frag f) {
    Frag fstar = star(p, f);
    return concat(f, fstar);
}

Frag question(Parser *p, Frag f) {
    Inst *split = emit(p, (Inst){I_SPLIT, 0, NULL, f.start, NULL, 0});
    Inst **out = malloc((f.nout + 1) * sizeof(Inst*));
    memcpy(out, f.out, f.nout * sizeof(Inst*));
    out[f.nout] = &split->y;
    free(f.out);
    return make_frag(split, out, f.nout + 1);
}

Frag parse_atom(Parser *p);
Frag parse_expr(Parser *p);

Frag parse_factor(Parser *p) {
    Frag f = parse_atom(p);

    while (p->current.type == TOK_STAR ||
           p->current.type == TOK_PLUS ||
           p->current.type == TOK_QUESTION) {
        TokenType t = p->current.type;
        advance(p);

        if (t == TOK_STAR) f = star(p, f);
        else if (t == TOK_PLUS) f = plus(p, f);
        else f = question(p, f);
    }

    return f;
}

Frag parse_sequence(Parser *p) {
    Frag f = parse_factor(p);

    while (p->current.type != TOK_EOF &&
           p->current.type != TOK_PIPE &&
           p->current.type != TOK_RPAREN) {
        Frag f2 = parse_factor(p);
        f = concat(f, f2);
    }

    return f;
}

Frag parse_expr(Parser *p) {
    Frag f = parse_sequence(p);

    while (p->current.type == TOK_PIPE) {
        advance(p);
        Frag f2 = parse_sequence(p);
        f = alternate(p, f, f2);
    }

    return f;
}

Frag parse_atom(Parser *p) {
    Frag f;
    Inst *i;

    switch (p->current.type) {
        case TOK_CHAR:
            i = emit(p, (Inst){I_CHAR, p->current.c, NULL, NULL, NULL, 0});
            advance(p);
            f = make_frag(i, malloc(sizeof(Inst*)), 1);
            f.out[0] = i;
            break;

        case TOK_DOT:
            i = emit(p, (Inst){I_CHARSET, 0, NULL, NULL, NULL, 0});
            i->charset = malloc(256);
            memset(i->charset, 1, 256);
            i->charset['\n'] = 0;
            advance(p);
            f = make_frag(i, malloc(sizeof(Inst*)), 1);
            f.out[0] = i;
            break;

        case TOK_CHARSET:
            i = emit(p, (Inst){I_CHARSET, 0, p->current.charset, NULL, NULL, 0});
            advance(p);
            f = make_frag(i, malloc(sizeof(Inst*)), 1);
            f.out[0] = i;
            break;

        case TOK_LPAREN:
            advance(p);
            f = parse_expr(p);
            if (p->current.type == TOK_RPAREN) advance(p);
            break;

        default:
            /* 空串 */
            i = emit(p, (Inst){I_JMP, 0, NULL, NULL, NULL, 0});
            f = make_frag(i, malloc(sizeof(Inst*)), 1);
            f.out[0] = i;
            break;
    }

    return f;
}

/* ============ 匹配引擎 ============ */

#define LIST_SIZE 256

int list_id = 0;

typedef struct {
    Inst *pc;
    const char *sp;
} Thread;

void add_thread(Thread *list, int *n, Inst *pc, const char *sp) {
    if (pc->op == I_SPLIT) {
        add_thread(list, n, pc->x, sp);
        add_thread(list, n, pc->y, sp);
    } else if (pc->op == I_JMP) {
        add_thread(list, n, pc->x, sp);
    } else {
        list[*n].pc = pc;
        list[*n].sp = sp;
        (*n)++;
    }
}

bool match_regex(Inst *prog, const char *text) {
    Thread current[LIST_SIZE], next[LIST_SIZE];
    int n_cur = 0, n_next = 0;

    add_thread(current, &n_cur, prog, text);

    for (const char *sp = text; ; sp++) {
        n_next = 0;

        for (int i = 0; i < n_cur; i++) {
            Thread *t = &current[i];

            switch (t->pc->op) {
                case I_CHAR:
                    if (*sp == t->pc->c) {
                        add_thread(next, &n_next, t->pc + 1, sp + 1);
                    }
                    break;

                case I_CHARSET:
                    if (*sp != '\0' && t->pc->charset[(unsigned char)*sp]) {
                        add_thread(next, &n_next, t->pc + 1, sp + 1);
                    }
                    break;

                case I_MATCH:
                    return true;

                default:
                    break;
            }
        }

        if (*sp == '\0') break;

        memcpy(current, next, n_next * sizeof(Thread));
        n_cur = n_next;
    }

    /* 检查接受状态 */
    for (int i = 0; i < n_cur; i++) {
        if (current[i].pc->op == I_MATCH) return true;
    }

    return false;
}

/* ============ 主程序 ============ */

bool regex_match(const char *pattern, const char *text) {
    Lexer lex;
    lexer_init(&lex, pattern);

    Parser parser;
    parser_init(&parser, &lex);

    Frag f = parse_expr(&parser);

    /* 添加匹配指令 */
    Inst *match = emit(&parser, (Inst){I_MATCH, 0, NULL, NULL, NULL, 0});
    for (int i = 0; i < f.nout; i++) {
        *f.out[i] = *match;
    }

    return match_regex(f.start, text);
}

int main(void) {
    printf("=== Regex Engine Demo ===\n\n");

    struct { const char *pat, *text; } tests[] = {
        {"abc", "abc"},
        {"a*b", "aaab"},
        {"a*b", "b"},
        {"a|b", "a"},
        {"a|b", "c"},
        {"colou?r", "color"},
        {"colou?r", "colour"},
        {"a+", "aaa"},
        {"a+", ""},
        {"\\d+", "12345"},
        {"\\w+", "hello_world"},
        {"a.*b", "axxxb"},
        {"[aeiou]+", "aei"},
        {"[^aeiou]+", "bcd"},
        {NULL, NULL}
    };

    for (int i = 0; tests[i].pat != NULL; i++) {
        bool result = regex_match(tests[i].pat, tests[i].text);
        printf("Pattern '%s' matches '%s': %s\n",
               tests[i].pat, tests[i].text,
               result ? "YES" : "NO");
    }

    return 0;
}
```

---

## 参考资源

### 经典文献

- **Ken Thompson (1968)**: "Regular Expression Search Algorithm"
- **Russ Cox (2007)**: "Regular Expression Matching Can Be Simple And Fast"
- **Russ Cox (2009)**: "Regular Expression Matching: the Virtual Machine Approach"

### 开源实现

| 项目 | 特点 | 链接 |
|------|------|------|
| RE2 | Google的DFA正则引擎 | github.com/google/re2 |
| PCRE2 | 功能最全面的NFA引擎 | pcre.org |
| Oniguruma | 支持多种语法 | github.com/kkos/oniguruma |
| Hyperscan | Intel的高性能多正则匹配 | github.com/intel/hyperscan |

---

*最后更新: 2026年3月*
