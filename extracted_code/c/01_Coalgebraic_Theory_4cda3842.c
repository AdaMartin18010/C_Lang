/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\02_Coalgebraic_Methods\01_Coalgebraic_Theory.md
 * Line: 226
 * Language: c
 * Block ID: 4cda3842
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// LTS的余代数实现
#include <stdlib.h>
#include <stdbool.h>

typedef struct LTSTransition LTSTransition;
typedef struct LTSState LTSState;

struct LTSTransition {
    char *action;
    LTSState *target;
    LTSTransition *next;
};

struct LTSState {
    char *name;
    LTSTransition *transitions;
};

LTSState *lts_create_state(const char *name) {
    LTSState *s = calloc(1, sizeof(LTSState));
    s->name = strdup(name);
    return s;
}

void lts_add_transition(LTSState *from, const char *action, LTSState *to) {
    LTSTransition *t = malloc(sizeof(LTSTransition));
    t->action = strdup(action);
    t->target = to;
    t->next = from->transitions;
    from->transitions = t;
}
