/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\02_Coalgebraic_Methods\01_Coalgebraic_Theory.md
 * Line: 285
 * Language: c
 * Block ID: e0dbedb6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 共归纳证明框架
typedef struct {
    void *left;
    void *right;
} BisimPair;

typedef struct {
    BisimPair *pairs;
    int count;
    int capacity;
} Bisimulation;

// 检查关系是否是互模拟
bool is_bisimulation(Bisimulation *R,
                      bool (*step)(void *, void *, Bisimulation *),
                      bool (*output_eq)(void *, void *)) {
    for (int i = 0; i < R->count; i++) {
        void *x = R->pairs[i].left;
        void *y = R->pairs[i].right;

        // 检查输出相等
        if (!output_eq(x, y)) return false;

        // 检查转移保持
        if (!step(x, y, R)) return false;
    }
    return true;
}
