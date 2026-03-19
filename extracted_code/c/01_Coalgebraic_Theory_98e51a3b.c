/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\02_Coalgebraic_Methods\01_Coalgebraic_Theory.md
 * Line: 571
 * Language: c
 * Block ID: 98e51a3b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 余代数模式在C中的函数式实现

// Anamorphism（展开）
typedef struct {
    void *(*coalgebra)(void *);
    void *seed;
} Unfold;

// Catamorphism（折叠）
typedef struct {
    void *(*algebra)(void *, void *);
    void *initial;
} Fold;

// 结合两者：Hylomorphism
void *hylo(Fold *fold, Unfold *unfold) {
    // 先展开再折叠，不构建中间结构
    void *state = unfold->seed;
    void *acc = fold->initial;

    while (1) {
        void *next = unfold->coalgebra(state);
        if (next == NULL) break;
        acc = fold->algebra(acc, next);
        state = next;
    }

    return acc;
}
