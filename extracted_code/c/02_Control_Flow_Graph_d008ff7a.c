/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\06_C_Assembly_Mapping\02_Control_Flow_Graph.md
 * Line: 87
 * Language: c
 * Block ID: d008ff7a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 支配树节点
typedef struct DomTreeNode {
    BasicBlock *bb;
    struct DomTreeNode *parent;
    struct DomTreeNode **children;
    int num_children;
} DomTreeNode;

// 迭代算法计算支配集
void compute_dominators(Function *func) {
    // 初始化
    for (每个基本块 B) {
        dom[B] = 所有基本块集合;
    }
    dom[entry] = {entry};

    // 迭代直到不动点
    bool changed;
    do {
        changed = false;
        for (每个基本块 B ≠ entry) {
            Set new_dom = 所有基本块集合;
            for (B的每个前驱P) {
                new_dom = intersect(new_dom, dom[P]);
            }
            new_dom = union(new_dom, {B});

            if (new_dom != dom[B]) {
                dom[B] = new_dom;
                changed = true;
            }
        }
    } while (changed);
}
