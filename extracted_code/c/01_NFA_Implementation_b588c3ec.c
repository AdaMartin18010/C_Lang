/*
 * Auto-generated from: 03_System_Technology_Domains\02_Regex_Engine\01_NFA_Implementation.md
 * Line: 75
 * Language: c
 * Block ID: b588c3ec
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
