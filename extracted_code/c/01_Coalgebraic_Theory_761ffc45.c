/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\02_Coalgebraic_Methods\01_Coalgebraic_Theory.md
 * Line: 190
 * Language: c
 * Block ID: 761ffc45
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// DFA的余代数实现
#include <stdbool.h>
#include <stdint.h>

#define ALPHABET_SIZE 26

typedef struct DFAState DFAState;

struct DFAState {
    bool is_accepting;
    DFAState *transitions[ALPHABET_SIZE];
};

DFAState *dfa_create_state(bool accepting) {
    DFAState *s = calloc(1, sizeof(DFAState));
    s->is_accepting = accepting;
    return s;
}

void dfa_add_transition(DFAState *from, char symbol, DFAState *to) {
    from->transitions[symbol - 'a'] = to;
}

// 余代数结构映射
bool dfa_output(DFAState *state) {
    return state->is_accepting;
}

DFAState *dfa_transition(DFAState *state, char symbol) {
    return state->transitions[symbol - 'a'];
}
