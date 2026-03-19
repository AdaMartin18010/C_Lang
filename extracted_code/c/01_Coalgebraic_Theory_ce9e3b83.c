/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\02_Coalgebraic_Methods\01_Coalgebraic_Theory.md
 * Line: 521
 * Language: c
 * Block ID: ce9e3b83
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 递归类型作为余代数

// 列表类型作为余代数
// List(A) = 1 + A × List(A)

typedef struct List List;
struct List {
    bool is_nil;
    union {
        struct { int head; List *tail; } cons;
    } data;
};

// 观察函数（余代数结构）
typedef struct {
    bool is_empty;
    int head;
    List *tail;
} ListObservation;

ListObservation observe_list(List *list) {
    ListObservation obs;
    obs.is_empty = list->is_nil;
    if (!list->is_nil) {
        obs.head = list->data.cons.head;
        obs.tail = list->data.cons.tail;
    }
    return obs;
}

// 余代数展开
void unfold_list(List **result,
                 void *state,
                 ListObservation (*step)(void *)) {
    ListObservation obs = step(state);
    if (obs.is_empty) {
        *result = NULL;
    } else {
        List *node = malloc(sizeof(List));
        node->is_nil = false;
        node->data.cons.head = obs.head;
        unfold_list(&node->data.cons.tail, obs.tail, step);
        *result = node;
    }
}
