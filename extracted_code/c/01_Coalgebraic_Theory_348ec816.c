/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\02_Coalgebraic_Methods\01_Coalgebraic_Theory.md
 * Line: 437
 * Language: c
 * Block ID: 348ec816
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Hennessy-Milner逻辑作为余代数

// 模态公式
typedef struct Formula {
    enum {
        FORM_TRUE,
        FORM_FALSE,
        FORM_AND,
        FORM_OR,
        FORM_NOT,
        FORM_DIAMOND,  // <a>φ
        FORM_BOX       // [a]φ
    } type;
    union {
        struct { struct Formula *left; struct Formula *right; } binary;
        struct Formula *unary;
        struct { char *action; struct Formula *inner; } modal;
    } data;
} Formula;

// 满足关系检查
bool satisfies(LTSState *state, Formula *formula) {
    switch (formula->type) {
        case FORM_TRUE:  return true;
        case FORM_FALSE: return false;
        case FORM_AND:
            return satisfies(state, formula->data.binary.left) &&
                   satisfies(state, formula->data.binary.right);
        case FORM_OR:
            return satisfies(state, formula->data.binary.left) ||
                   satisfies(state, formula->data.binary.right);
        case FORM_NOT:
            return !satisfies(state, formula->data.unary);
        case FORM_DIAMOND: {
            // <a>φ : 存在a转移到达满足φ的状态
            char *a = formula->data.modal.action;
            Formula *phi = formula->data.modal.inner;
            for (LTSTransition *t = state->transitions; t; t = t->next) {
                if (strcmp(t->action, a) == 0 && satisfies(t->target, phi)) {
                    return true;
                }
            }
            return false;
        }
        case FORM_BOX: {
            // [a]φ : 所有a转移都到达满足φ的状态
            char *a = formula->data.modal.action;
            Formula *phi = formula->data.modal.inner;
            for (LTSTransition *t = state->transitions; t; t = t->next) {
                if (strcmp(t->action, a) == 0 && !satisfies(t->target, phi)) {
                    return false;
                }
            }
            return true;
        }
    }
    return false;
}
