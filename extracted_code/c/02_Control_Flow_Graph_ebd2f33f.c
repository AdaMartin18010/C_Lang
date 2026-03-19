/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\06_C_Assembly_Mapping\02_Control_Flow_Graph.md
 * Line: 272
 * Language: c
 * Block ID: ebd2f33f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 合并条件：
// - A 有唯一后继 B
// - B 有唯一前驱 A
// - B 不是 entry

bool can_merge(BasicBlock *A, BasicBlock *B) {
    return (A->num_succs == 1 && A->succs[0] == B &&
            B->num_preds == 1 && B->preds[0] == A &&
            B != entry);
}

void merge_blocks(BasicBlock *A, BasicBlock *B) {
    // 将B的指令追加到A
    A->last->next = B->first;
    A->last = B->last;

    // 更新A的后继
    A->succs = B->succs;
    A->num_succs = B->num_succs;

    // 删除B
    delete_basic_block(B);
}
