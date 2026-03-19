/*
 * Auto-generated from: 03_System_Technology_Domains\02_Regex_Engine\README.md
 * Line: 148
 * Language: c
 * Block ID: f4410b1b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
