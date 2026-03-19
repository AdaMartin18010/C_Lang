/*
 * Auto-generated from: 03_System_Technology_Domains\02_Regex_Engine\01_NFA_Implementation.md
 * Line: 138
 * Language: c
 * Block ID: bc9bbac1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
